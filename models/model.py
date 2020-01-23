# Copyright (c) 2019, NVIDIA CORPORATION.  All rights reserved.
#
# NVIDIA CORPORATION and its licensors retain all intellectual property
# and proprietary rights in and to this software, related documentation
# and any modifications thereto.  Any use, reproduction, disclosure or
# distribution of this software and related documentation without an express
# license agreement from NVIDIA CORPORATION is strictly prohibited.
import sys
import os


# For my machine
BASE_DIR = os.path.dirname(__file__)
sys.path.append(os.path.join(BASE_DIR, '../../pointnet2/utils/'))
# For docker
sys.path.append('/pointnet2/utils')

import tf_util
from pointnet_util import pointnet_sa_module, pointnet_sa_module_msg, pointnet_fp_module
import tf_utils
from tf_utils import get_shape
import tensorflow as tf
import os

import numpy as np
import trimesh.transformations as tra

# For my machine
BASE_DIR = os.path.dirname(__file__)
sys.path.append(BASE_DIR)
sys.path.append(os.path.join(BASE_DIR, '../pointnet2/utils/'))
sys.path.append(os.path.join(BASE_DIR, '../'))


def base_network(l0_xyz,
                 l0_points,
                 is_training,
                 bn_decay,
                 scale,
                 pointnet_radius=0.02,
                 pointnet_nclusters=128):
    """
      Backbone model used for encoder, decoder, and evaluator.
    """
    l1_xyz, l1_points, _ = pointnet_sa_module(
        l0_xyz, l0_points, npoint=pointnet_nclusters, radius=pointnet_radius,
        nsample=64, mlp=[
                                              64 * scale, 64 * scale, 128 * scale], mlp2=None, group_all=False, is_training=is_training, bn_decay=bn_decay, scope='ssg-layer1')
    l2_xyz, l2_points, _ = pointnet_sa_module(
        l1_xyz, l1_points, npoint=32, radius=0.04, nsample=128, mlp=[
        128 * scale, 128 * scale, 256 * scale], mlp2=None, 
        group_all=False, is_training=is_training, bn_decay=bn_decay, 
        scope='ssg-layer2')
    _, l3_points, _ = pointnet_sa_module(
        l2_xyz, l2_points, npoint=None, radius=None, nsample=None, mlp=[
        256 * scale, 256 * scale, 512 * scale], mlp2=None, group_all=True,
        is_training=is_training, bn_decay=bn_decay, scope='ssg-layer3')

    # Fully connected layers
    batch_size = get_shape(l0_xyz)[0]
    net = tf.reshape(l3_points, [batch_size, -1])
    net = tf_util.fully_connected(
        net,
        1024 * scale,
        bn=True,
        is_training=is_training,
        scope='fc1',
        bn_decay=bn_decay)
    net = tf_util.fully_connected(
        net,
        1024 * scale,
        bn=True,
        is_training=is_training,
        scope='fc2',
        bn_decay=bn_decay)
    return net


def model_with_confidence(
    pc,
    latent,
    is_training,
    bn_decay,
    is_encoder,
    latent_size=None,
    scale=1,
    merge_pcs=False,
    pointnet_radius=0.02,
    pointnet_nclusters=128):
    """
      If is_encoder=True, it creates a model that outputs grasp score and 
      grasp confidence. Grasp confidence is the confidence of the network
      in the predicted scores.
    """

    assert (~isinstance(is_training, bool))
    if not is_encoder:
        if merge_pcs:
            raise ValueError(
                'unless in encoder mode, merge_pcs should be False!!!')

    l0_xyz = pc
    l0_points = tf.concat([l0_xyz, latent], -1)

    if is_encoder and merge_pcs:
        grasp_rt = latent
        grasp_shape = get_shape(grasp_rt)

        print('encoder: merge_pc: grasp_shape: ', grasp_shape)
        if len(grasp_shape) != 3 or grasp_shape[1] != 4 or grasp_shape[2] != 4:
            raise ValueError('invalid grasp shape '.format(grasp_shape))

        gripper_pc = tf.matmul(tf_utils.get_gripper_pc(get_shape(
            pc)[0], -1), grasp_rt, transpose_a=False, transpose_b=True)
        gripper_pc = tf.slice(gripper_pc, [0, 0, 0], [-1, -1, 3])
        print(
            'gripper_pc = {}, pc = {}'.format(
                get_shape(gripper_pc),
                get_shape(pc)))

        l0_xyz, l0_points = merge_pc_and_gripper_pc(pc, gripper_pc)

    print('l0_xyz = {} l0_points = {}'.format(
        get_shape(l0_xyz), get_shape(l0_points)))

    net = base_network(
        l0_xyz,
        l0_points,
        is_training,
        bn_decay,
        scale,
        pointnet_radius,
        pointnet_nclusters)

    if is_encoder:
        assert(latent_size is not None)
        mean = tf_util.fully_connected(
            net, latent_size, activation_fn=None, scope='fc_mean')
        logvar = tf_util.fully_connected(
            net, latent_size, activation_fn=None, scope='fc_var')

        return tf.concat((mean, logvar), -1)
    else:
        q = tf_util.fully_connected(net, 4, activation_fn=None, scope='fc_q')
        q = tf.nn.l2_normalize(q, -1)
        t = tf_util.fully_connected(net, 3, activation_fn=None, scope='fc_t')
        confidence = tf_util.fully_connected(
            net, 1, activation_fn=None, scope='fc_conf')
        confidence = tf.nn.sigmoid(confidence)

        return q, t, confidence


def merge_pc_and_gripper_pc(
        pc,
        gripper_pc,
        instance_mode=0,
        pc_latent=None,
        gripper_pc_latent=None):
    """
    Merges the object point cloud and gripper point cloud and
    adds a binary auxilary feature that indicates whether each point
    belongs to the object or to the gripper.
    """

    pc_shape = get_shape(pc)
    gripper_shape = get_shape(gripper_pc)
    assert(len(pc_shape) == 3)
    assert(len(gripper_shape) == 3)
    assert(pc_shape[0] == gripper_shape[0])

    npoints = get_shape(pc)[1]
    batch_size = tf.shape(pc)[0]

    if instance_mode == 1:
        assert pc_shape[-1] == 3
        latent_dist = [pc_latent, gripper_pc_latent]
        latent_dist = tf.concat(latent_dist, 1)

    l0_xyz = tf.concat((pc, gripper_pc), 1)
    labels = [
        tf.ones(
            (get_shape(pc)[1], 1), dtype=tf.float32), tf.zeros(
            (get_shape(gripper_pc)[1], 1), dtype=tf.float32)]
    labels = tf.concat(labels, 0)
    labels = tf.expand_dims(labels, 0)
    labels = tf.tile(labels, [batch_size, 1, 1])

    if instance_mode == 1:
        l0_points = tf.concat([l0_xyz, latent_dist, labels], -1)
    else:
        l0_points = tf.concat([l0_xyz, labels], -1)

    return l0_xyz, l0_points


def evaluator_model(
        pc,
        gripper_pc,
        is_training,
        bn_decay,
        scale=1,
        instance_mode=0,
        pc_latent=None,
        gripper_pc_latent=None):
    """
      Evaluator model that predicts a score for each grasp.
    """
    l0_xyz, l0_points = merge_pc_and_gripper_pc(
        pc, gripper_pc, instance_mode, pc_latent, gripper_pc_latent)
    net = base_network(
        l0_xyz,
        l0_points,
        is_training,
        bn_decay,
        scale)  # Get features of the pc
    predictions_logits = tf_util.fully_connected(
        net, 2, activation_fn=None, scope='fc_collision')
    confidence = tf_util.fully_connected(
        net, 1, activation_fn=None, scope='fc_collision_confidence')
    confidence = tf.nn.sigmoid(confidence)

    return predictions_logits, confidence


def kl_divergence(mu, log_sigma):
    """
      Computes the kl divergence for batch of mu and log_sigma.
    """
    return -.5 * tf.reduce_sum(1. + log_sigma -
                               tf.square(mu) - tf.exp(log_sigma), axis=-1)



def control_point_l1_loss_better_than_threshold(pred_control_points,
                                                gt_control_points,
                                                confidence,
                                                confidence_threshold):
    npoints = get_shape(pred_control_points)[1]
    mask = tf.greater_equal(confidence, confidence_threshold)
    mask_ratio = tf.reduce_mean(tf.to_float(mask))
    mask = tf.tile(mask, [1, npoints])
    p1 = tf.boolean_mask(pred_control_points, mask)
    p2 = tf.boolean_mask(gt_control_points, mask)

    return control_point_l1_loss(p1, p2), mask_ratio


def accuracy_better_than_threshold(
        pred_success_logits,
        gt,
        confidence,
        confidence_threshold):
    """
      Computes average precision for the grasps with confidence > threshold.
    """
    pred_classes = tf.cast(tf.argmax(pred_success_logits, -1), tf.int32)
    correct = tf.to_float(tf.equal(pred_classes, gt))
    mask = tf.squeeze(
        tf.to_float(
            tf.greater_equal(
                confidence, confidence_threshold)), -1)

    gt = tf.to_float(gt)
    positive_acc = tf.reduce_sum(
        correct * mask * gt) / tf.maximum(tf.reduce_sum(mask * gt), 1.)
    negative_acc = tf.reduce_sum(
        correct * mask * (1. - gt)) / tf.maximum(
            tf.reduce_sum(mask * (1. - gt)), 1.)

    return 0.5 * (positive_acc +
                  negative_acc), tf.reduce_sum(mask) / get_shape(gt)[0]


def control_point_l1_loss(
        pred_control_points,
        gt_control_points,
        confidence=None,
        confidence_weight=None):
    """
      Computes the l1 loss between the predicted control points and the
      groundtruth contorl points on the gripper.
    """
    confidence_term = tf.constant(0, dtype=tf.float32)
    print('control_point_l1_loss',
          get_shape(pred_control_points),
          get_shape(gt_control_points))
    error = tf.reduce_sum(tf.abs(pred_control_points - gt_control_points), -1)
    error = tf.reduce_mean(error, -1)
    if confidence is not None:
        assert(confidence_weight is not None)
        error *= confidence
        confidence_term = tf.reduce_mean(
            tf.log(
                tf.maximum(
                    confidence,
                    1e-10))) * confidence_weight
        print('confidence_term = ', get_shape(confidence_term))

    print('l1_error = {}'.format(get_shape(error)))
    if confidence is None:
        return tf.reduce_mean(error)
    else:
        return tf.reduce_mean(error), -confidence_term


def classification_with_confidence_loss(
        pred_logit, gt, confidence, confidence_weight):
    """
      Computes the cross entropy loss and confidence term that penalizes
      outputing zero confidence. Returns cross entropy loss and the confidence
      regularization term.
    """
    classification_loss = tf.nn.sparse_softmax_cross_entropy_with_logits(
        logits=pred_logit, labels=gt)
    classification_loss = tf.reduce_mean(classification_loss)
    confidence_term = tf.reduce_mean(tf.log(tf.maximum(confidence, 1e-10)))

    return classification_loss, -confidence_term


def min_distance_loss(
    pred_control_points, 
    gt_control_points,
    confidence=None, 
    confidence_weight=None,
    threshold=None,
    ):
    """
    Computes the minimum distance (L1 distance)between each gt control point 
    and any of the predicted control points.

    Args: 
      pred_control_points: tensor of (N_pred, M, 4) shape. N is the number of
        grasps. M is the number of points on the gripper.
      gt_control_points: (N_gt, M, 4)
      confidence: tensor of N_pred, tensor for the confidence of each 
        prediction.
      confidence_weight: float, the weight for confidence loss.
    """
    pred_shape = get_shape(pred_control_points)
    gt_shape = get_shape(gt_control_points)

    if len(pred_shape) != 3:
        raise ValueError("pred_control_point should have len of 3. {}".format(pred_shape))
    if len(gt_shape) != 3:
        raise ValueError("gt_control_point should have len of 3. {}".format(gt_shape))
    if np.any([p != gt for i, (p, gt) in enumerate(zip(pred_shape, gt_shape)) if i > 0]):
        raise ValueError("shapes do no match {} != {}".format(
            pred_shape, gt_shape))

    # N_pred x Ngt x M x 3
    error = tf.expand_dims(pred_control_points, 1) - tf.expand_dims(gt_control_points, 0)
    error = tf.reduce_sum(tf.abs(error), -1) # L1 distance of error (N_pred, N_gt, M)
    error = tf.reduce_mean(error, -1) # average L1 for all the control points. (N_pred, N_gt)
    min_distance_error = tf.reduce_min(error, 0) # take the min distance for each gt control point. (N_gt)
    #print('min_distance_error', get_shape(min_distance_error))
    if confidence is not None:
        closest_index = tf.argmin(error, 0) # (N_gt)
        #print('closest_index', get_shape(closest_index))
        selected_confidence = tf.one_hot(closest_index, axis=-1, depth=pred_shape[0]) # (N_gt, N_pred)
        #print('selected_confidence', selected_confidence)
        selected_confidence *= tf.expand_dims(confidence, 0)
        #print('selected_confidence', selected_confidence)
        selected_confidence = tf.reduce_sum(selected_confidence, -1) # N_gt
        #print('selected_confidence', selected_confidence)
        min_distance_error *= selected_confidence
        confidence_term = tf.reduce_mean(
            tf.log(
                tf.maximum(
                    confidence,
                    1e-4
                ))) * confidence_weight
    else:
        confidence_term = 0.
    
    return tf.reduce_mean(min_distance_error), -confidence_term

def min_distance_better_than_threshold(pred_control_points,
                                       gt_control_points,
                                       confidence,
                                       confidence_threshold):
    error = tf.expand_dims(pred_control_points, 1) - tf.expand_dims(gt_control_points, 0)
    error = tf.reduce_sum(tf.abs(error), -1) # L1 distance of error (N_pred, N_gt, M)
    error = tf.reduce_mean(error, -1) # average L1 for all the control points. (N_pred, N_gt)
    error = tf.reduce_min(error, -1) # (B, N_pred)
    mask = tf.to_float(tf.greater_equal(confidence, confidence_threshold))
    mask = tf.squeeze(mask, axis=-1)
    
    return tf.reduce_mean(tf.boolean_mask(error, mask)), tf.reduce_mean(mask)

