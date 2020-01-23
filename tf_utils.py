# Copyright (c) 2019, NVIDIA CORPORATION.  All rights reserved.
#
# NVIDIA CORPORATION and its licensors retain all intellectual property
# and proprietary rights in and to this software, related documentation
# and any modifications thereto.  Any use, reproduction, disclosure or
# distribution of this software and related documentation without an express
# license agreement from NVIDIA CORPORATION is strictly prohibited.
from __future__ import print_function

import math
import numpy as np
import tensorflow as tf
import time
import trimesh.transformations as tra
import os

GRIPPER_PC = np.load(
    'gripper_models/panda_pc.npy', allow_pickle=True).item()['points']
GRIPPER_PC[:, 3] = 1.


def get_shape(x):
    """
      Gets the shape of the tensor x.
    """
    return x.get_shape().as_list()


def count_nan(x):
    """
      Debug function: counts the nan values in tensor x.
    """
    isnan = tf.cast(tf.is_nan(x), tf.int32)
    isnan = tf.reshape(isnan, [-1])
    return tf.reduce_sum(isnan)


def get_gripper_pc(batch_size, npoints, use_tf=True):
    """
      Returns a numpy array or a tensor of shape (batch_size x npoints x 4).
      Represents gripper with the sepcified number of points.
      use_tf: switches between output tensor or numpy array.
    """
    output = np.copy(GRIPPER_PC)
    if npoints != -1:
        assert(
            npoints > 0 and npoints <= output.shape[0]), 'gripper_pc_npoint is too large {} > {}'.format(
            npoints, output.shape[0])
        output = output[:npoints]
        output = np.expand_dims(output, 0)
    else:
        raise ValueError('npoints should not be -1.')
        
    if use_tf:
        output = tf.convert_to_tensor(output, tf.float32)
        output = tf.tile(output, [batch_size, 1, 1])
        return output
    else:
        output = np.tile(output, [batch_size, 1, 1])

    return output


def get_control_point_tensor(batch_size, use_tf=True):
    """
      Outputs a tensor of shape (batch_size x 6 x 3).
      use_tf: switches between outputing a tensor and outputing a numpy array.
    """
    control_points = np.load('gripper_control_points/panda.npy')[:, :3]
    control_points = [[0, 0, 0], [0, 0, 0], control_points[0, :],
                      control_points[1, :], control_points[-2, :], control_points[-1, :]]
    control_points = np.asarray(control_points, dtype=np.float32)
    control_points = np.tile(
        np.expand_dims(
            control_points, 0), [
            batch_size, 1, 1])

    if use_tf:
        return tf.convert_to_tensor(control_points)

    return control_points


def transform_control_points(
        gt_grasps,
        batch_size,
        mode='qt',
        scope='transform_gt_control_points'):
    """
      Transforms canonical points using gt_grasps.
      mode = 'qt' expects gt_grasps to have (batch_size x 7) where each 
        element is concatenation of quaternion and translation for each
        grasps.
      mode = 'rt': expects to have shape (batch_size x 4 x 4) where
        each element is 4x4 transformation matrix of each grasp.
    """
    assert(mode == 'qt' or mode == 'rt'), mode
    grasp_shape = get_shape(gt_grasps)
    if mode == 'qt':
        assert(len(grasp_shape) == 2), grasp_shape
        assert(grasp_shape[-1] == 7), grasp_shape
        with tf.variable_scope(scope):
            control_points = get_control_point_tensor(batch_size)
            num_control_points = get_shape(control_points)[1]
            input_gt_grasps = gt_grasps
            gt_grasps = tf.tile(
                tf.expand_dims(
                    input_gt_grasps, 1), [
                    1, num_control_points, 1])
            gt_q = tf.slice(
                gt_grasps, [
                    0, 0, 0], [
                    get_shape(gt_grasps)[0], get_shape(gt_grasps)[1], 4])
            gt_t = tf.slice(
                gt_grasps, [
                    0, 0, 4], [
                    get_shape(gt_grasps)[0], get_shape(gt_grasps)[1], 3])

            gt_control_points = rotate_point_by_quaternion(
                control_points, gt_q)
            gt_control_points += gt_t

            return gt_control_points
    else:
        assert(len(grasp_shape) == 3), grasp_shape
        assert(grasp_shape[1] == 4 and grasp_shape[2] == 4), grasp_shape
        with tf.variable_scope(scope):
            control_points = get_control_point_tensor(batch_size)
            shape = get_shape(control_points)
            ones = tf.ones((shape[0], shape[1], 1), dtype=tf.float32)
            control_points = tf.concat((control_points, ones), -1)
            return tf.matmul(
                control_points,
                gt_grasps,
                transpose_a=False,
                transpose_b=True)


def quaternion_mult(Q, R):
    """
      Computes the multiplication of quaternions Q and R.
    """
    Q_shape = Q.get_shape().as_list()
    R_shape = R.get_shape().as_list()
    assert (Q_shape[-1] == 4)
    assert (R_shape[-1] == 4)
    q = tf.split(Q, 4, axis=-1)
    r = tf.split(R, 4, axis=-1)
    outputs_list = [
        r[0] * q[0] - r[1] * q[1] - r[2] * q[2] - r[3] * q[3],
        r[0] * q[1] + r[1] * q[0] - r[2] * q[3] + r[3] * q[2],
        r[0] * q[2] + r[1] * q[3] + r[2] * q[0] - r[3] * q[1],
        r[0] * q[3] - r[1] * q[2] + r[2] * q[1] + r[3] * q[0]
    ]
    outputs = tf.concat(outputs_list, axis=-1)
    return outputs


def conj_quaternion(q):
    """
      Conjugate of quaternion q.
    """
    q_conj = tf.split(q, 4, axis=-1)
    q_conj = tf.concat(
        [q_conj[0], -q_conj[1], -q_conj[2], -q_conj[3]], axis=-1)
    return q_conj


def rotate_point_by_quaternion(point, q):
    """
      Takes in points with shape of (batch_size x n x 3) and quaternions with
      shape of (batch_size x n x 4) and returns a tensor with shape of 
      (batch_size x n x 3) which is the rotation of the point with quaternion
      q. 
    """
    shape = point.get_shape().as_list()
    q_shape = q.get_shape().as_list()

    assert(
        len(shape) == 3), 'point shape = {} q shape = {}'.format(
        shape, q_shape)
    assert(shape[-1] == 3), 'point shape = {} q shape = {}'.format(shape, q_shape)
    assert(
        len(q_shape) == 3), 'point shape = {} q shape = {}'.format(
        shape, q_shape)
    assert(q_shape[-1] ==
           4), 'point shape = {} q shape = {}'.format(shape, q_shape)
    assert(
        q_shape[1] == shape[1]), 'point shape = {} q shape = {}'.format(
        shape, q_shape)

    q_conj = conj_quaternion(q)
    r = tf.concat(
        [tf.zeros((shape[0], shape[1], 1), dtype=point.dtype), point], axis=-1)
    final_point = quaternion_mult(quaternion_mult(q, r), q_conj)
    final_output = tf.slice(final_point, [0, 0, 1], shape, name='sliceeeeeee')
    return final_output


class QuaternionTest(tf.test.TestCase):
    def test_mult(self):
        np.random.seed(int(time.time()))
        batch_size = 30
        control_points = 50
        a = np.random.rand(batch_size, control_points, 4)
        b = np.random.rand(batch_size, control_points, 4)
        norm_a = np.sqrt(np.sum(a * a, axis=-1))
        norm_b = np.sqrt(np.sum(b * b, axis=-1))
        a /= np.tile(np.expand_dims(norm_a, -1), [1, 1, 4])
        b /= np.tile(np.expand_dims(norm_b, -1), [1, 1, 4])
        output = np.zeros((batch_size, control_points, 4), dtype=np.float32)
        for bindex in range(batch_size):
            for c in range(control_points):
                output[bindex, c, :] = tra.quaternion_multiply(
                    a[bindex, c, :], b[bindex, c, :])

        ta = tf.convert_to_tensor(a)
        tb = tf.convert_to_tensor(b)
        tf_output = quaternion_mult(ta, tb)

        ok = True
        with self.test_session():
            if np.all(np.abs(tf_output.eval() - output) < 1e-4):
                print('----------> Mult passed')
            else:
                raise ValueError(
                    'did not match {} != {}'.format(
                        tf_output.eval(), output))

    def test_rotation(self):
        np.random.seed(int(time.time()))
        batch_size = 30
        control_points = 16

        rot_matrix = np.zeros(
            (batch_size, control_points, 3, 3), dtype=np.float32)
        quat_matrix = np.zeros(
            (batch_size, control_points, 4), dtype=np.float32)
        points = np.random.rand(batch_size, control_points, 3)

        rotated_points = np.random.rand(batch_size, control_points, 3)
        for b in range(batch_size):
            for c in range(control_points):
                angles = np.random.uniform(
                    low=0, high=math.pi * 2., size=[3, ])
                rot_matrix[b, c, :, :] = tra.euler_matrix(
                    angles[0], angles[1], angles[2])[:3, :3]
                quat_matrix[b, c, :] = tra.quaternion_from_euler(
                    angles[0], angles[1], angles[2])

                rotated_points[b, c, :] = np.matmul(
                    rot_matrix[b, c, :, :], points[b, c, :])

        tf_rotated_points = rotate_point_by_quaternion(
            tf.convert_to_tensor(
                points, dtype=tf.float32), tf.convert_to_tensor(
                quat_matrix, dtype=tf.float32))

        with self.test_session():
            if np.all(
                np.abs(
                    tf_rotated_points.eval() -
                    rotated_points) < 1e-4):
                print('----------> Rotation passed')
            else:
                raise ValueError(
                    'test rotatation did not match {} != {}'.format(
                        tf_rotated_points.eval(), rotated_points))


def tf_rotation_matrix(az, el, th, batched=False):
    if batched:
        cx = tf.cos(tf.reshape(az, [-1, 1]))
        cy = tf.cos(tf.reshape(el, [-1, 1]))
        cz = tf.cos(tf.reshape(th, [-1, 1]))
        sx = tf.sin(tf.reshape(az, [-1, 1]))
        sy = tf.sin(tf.reshape(el, [-1, 1]))
        sz = tf.sin(tf.reshape(th, [-1, 1]))

        ones = tf.ones_like(cx)
        zeros = tf.zeros_like(cx)

        rx = tf.concat([ones, zeros, zeros, zeros,
                        cx, -sx, zeros, sx, cx], axis=-1)
        ry = tf.concat([cy, zeros, sy, zeros, ones,
                        zeros, -sy, zeros, cy], axis=-1)
        rz = tf.concat([cz, -sz, zeros, sz, cz, zeros,
                        zeros, zeros, ones], axis=-1)

        rx = tf.reshape(rx, [-1, 3, 3])
        ry = tf.reshape(ry, [-1, 3, 3])
        rz = tf.reshape(rz, [-1, 3, 3])

        return tf.matmul(rz, tf.matmul(ry, rx))
    else:
        cx = tf.cos(az)
        cy = tf.cos(el)
        cz = tf.cos(th)
        sx = tf.sin(az)
        sy = tf.sin(el)
        sz = tf.sin(th)

        rx = tf.to_float(
            tf.stack([[1., 0., 0.], [0, cx, -sx], [0, sx, cx]], axis=0))
        ry = tf.to_float(
            tf.stack([[cy, 0, sy], [0, 1, 0], [-sy, 0, cy]], axis=0))
        rz = tf.to_float(
            tf.stack([[cz, -sz, 0], [sz, cz, 0], [0, 0, 1]], axis=0))

        return tf.matmul(rz, tf.matmul(ry, rx))


if __name__ == '__main__':
    tf.test.main()
