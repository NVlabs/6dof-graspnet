# Copyright (c) 2019, NVIDIA CORPORATION.  All rights reserved.
#
# NVIDIA CORPORATION and its licensors retain all intellectual property
# and proprietary rights in and to this software, related documentation
# and any modifications thereto.  Any use, reproduction, disclosure or
# distribution of this software and related documentation without an express
# license agreement from NVIDIA CORPORATION is strictly prohibited.
from __future__ import absolute_import
from __future__ import print_function

import argparse
import random
import copy
import json
import math
import yaml
import signal
import threading
import os
import sys
import time

import numpy as np
# import cv2
import tensorflow as tf
from tqdm import tqdm
import trimesh

try:
    from IPython import embed
except:
    pass

try:
    # mayavi is not set up in docker.
    from visualization_utils import draw_scene
    HEADLESS = False
except:
    import horovod.tensorflow as hvd
    HEADLESS = True

import models.model
from tf_utils import get_shape
import tf_utils
import trimesh.transformations as tra
import grasp_data_reader
import sample
from easydict import EasyDict as edict

from tensorflow.python.training.summary_io import SummaryWriterCache

from grasp_data_reader import evaluate_grasps
from utils import get_files, set_seed, make_parser

tf.logging.set_verbosity(tf.logging.INFO)

CONFIDENCES = [0.2, 0.4, 0.6, 0.7, 0.8, 0.9]

current_index = 0
epoch_count = 0
lock = threading.Lock()

def get_vae_data_dict(first_dimension, args, files, pcreader, scope='vae'):
    """
    Returns the dictionary of input tensors that is used for training VAE.

    Args:
      first_dimension: int, it is equal to 
        num_objects_per_batch x num_grasps_per_object.
      args: arguments that are used for training.
      files: list of str, list of training files.
      pcreader: PointCloudReader.
    """
    OUTPUT_SHAPES = {
        'pc': [first_dimension, args.npoints, 4],
        'grasp_rt': [first_dimension, 4, 4],
        'pc_pose': [first_dimension, 4, 4],
        'cad_path': [first_dimension],
        'cad_scale': [first_dimension],
        'quality': [first_dimension],
    }

    OUTPUT_KEYS = sorted(list(OUTPUT_SHAPES.keys()))
    OUTPUT_TYPES = []
    for k in OUTPUT_KEYS:
        if k == 'cad_path':
            OUTPUT_TYPES.append(tf.string)
        else:
            OUTPUT_TYPES.append(tf.float32)

    def get_grasping_data_func():
        global current_index, epoch_count, lock, all_poses
        with lock:
            output_dict = {k: [] for k in OUTPUT_SHAPES}
            for _ in range(args.num_objects_per_batch):
                while True:
                    file_name = files[current_index]
                    no_positive_grasps = False
                    try:
                        output = pcreader.get_vae_data(file_name)
                    except grasp_data_reader.NoPositiveGraspsException:
                        no_positive_grasps = True

                    current_index += 1
                    if current_index == len(files):
                        random.shuffle(files)
                        epoch_count += 1
                        current_index = 0

                    if no_positive_grasps:
                        print('skipping {} because no positive grasps'.format(file_name))
                        continue
                    else:
                        break
                
                output_dict['pc'].append(output[0])
                output_dict['grasp_rt'].append(output[1])
                output_dict['pc_pose'].append(output[2])
                output_dict['cad_path'].append(output[3])
                output_dict['cad_scale'].append(output[4])
                output_dict['quality'].append(output[5])

            for k in output_dict:
                output_dict[k] = np.asarray(output_dict[k])
                try:
                    output_dict[k] = np.reshape(output_dict[k], OUTPUT_SHAPES[k])
                except Exception as e:
                    print('{} ===> {} {}'.format(k, output_dict[k].shape, OUTPUT_SHAPES[k]))
                    print(e)

            output_list = []
            for k in OUTPUT_KEYS:
                output_list.append(output_dict[k])

            return output_list

    data_list = tf.py_func(
        get_grasping_data_func,
        [],
        OUTPUT_TYPES,
        stateful=True,
        name='vae_data_reader'
    )
    data_dict = {'{}_'.format(scope)+k: v for k, v in zip(OUTPUT_KEYS, data_list)}
    for k, shape in OUTPUT_SHAPES.items():
        data_dict['{}_'.format(scope)+k].set_shape(shape)
    return data_dict

def get_evaluator_data_dict(first_dimension, args, files, pcreader, scope='evaluator'):
    """
    Returns dictionary for training evaluator.

    Args:
      first_dimension: int, num_objects_per_batch x num_grasps_per_object.
      args: arguments used for training.
      files: list of string, contains path for the training.
      pcreader: PointCloudReader.
    """
    global current_index, epoch_count, lock, all_poses

    OUTPUT_SHAPES = {
        'pc': [first_dimension, args.npoints, 4],
        'grasp_rt': [first_dimension, 4, 4],
        'label': [first_dimension], # Binary, success or not
        'grasp_quality': [first_dimension], # For debugging only
        'pc_pose': [first_dimension, 4, 4],
        'cad_path': [first_dimension],
        'cad_scale': [first_dimension],
    }

    OUTPUT_KEYS = sorted(list(OUTPUT_SHAPES.keys()))
    OUTPUT_TYPES = []
    for k in OUTPUT_KEYS:
        if k == 'cad_path':
            OUTPUT_TYPES.append(tf.string)
        elif k == 'label':
            OUTPUT_TYPES.append(tf.int32)
        else:
            OUTPUT_TYPES.append(tf.float32)


    def get_evaluator_data_func():
        global current_index, epoch_count, lock, all_poses
        with lock:
            output_dict = {k: [] for k in OUTPUT_SHAPES}
            for _ in range(args.num_objects_per_batch):
                while True:
                    file_name = files[current_index]
                    no_positive_grasps = False
                    try:
                        output = pcreader.get_evaluator_data(file_name)
                    except grasp_data_reader.NoPositiveGraspsException:
                        no_positive_grasps = True

                    current_index += 1
                    if current_index == len(files):
                        random.shuffle(files)
                        epoch_count += 1
                        current_index = 0
                    
                    if no_positive_grasps:
                        print('skipping {} because no positive grasps'.format(file_name))
                        continue
                    else:
                        break
                
                output_dict['pc'].append(output[0])
                output_dict['grasp_rt'].append(output[1])
                output_dict['label'].append(output[2])
                output_dict['grasp_quality'].append(output[3])
                output_dict['pc_pose'].append(output[4])
                output_dict['cad_path'].append(output[5])
                output_dict['cad_scale'].append(output[6])

            for k in output_dict:
                output_dict[k] = np.asarray(output_dict[k])
                try:
                    output_dict[k] = np.reshape(output_dict[k], OUTPUT_SHAPES[k])
                except Exception as e:
                    print('{} =====> {} {}'.format(k, output_dict[k].shape, OUTPUT_SHAPES[k]))
                    print(e)
                    #raise ValueError("asd")

            output_list = []
            for k in OUTPUT_KEYS:
                output_list.append(output_dict[k])

            return output_list

    # Takes in the function that generate the data dict and converts it to a tf operation
    data_list = tf.py_func(
        get_evaluator_data_func,
        [],
        OUTPUT_TYPES,
        stateful=True,
        name='evaluator_data_reader'
    )
    data_dict = {'{}_'.format(scope)+k: v for k, v in zip(OUTPUT_KEYS, data_list)}
    for k, shape in OUTPUT_SHAPES.items():
        data_dict['{}_'.format(scope)+k].set_shape(shape)

    data_dict['{}_gt_control_points'.format(scope)] = tf_utils.transform_control_points(
        data_dict['{}_grasp_rt'.format(scope)], first_dimension, mode='rt'
    )

    return data_dict


def verify_tensor_size(t, expected_shape):
    """
    Checks whether input tensor t, has the expected_shape.

    Args:
      t: input tensor
      expected_shape: list of int indicating the expected shape.
    """
    shape = get_shape(t)
    if len(shape) != len(expected_shape):
        raise ValueError('shape do not match : {} != {}'.format(shape, expected_shape))

    if np.any(np.asarray(shape) != np.asarray(expected_shape)):
        raise ValueError('shape do not match : {} != {}'.format(shape, expected_shape))


def build_evaluator_ops(data_dict, args, scope='evaluator', npoints=-1):
    """
    Builds all the tf ops necessary for training/evaluating the evaluator
    network.
    
    Args:
      data_dict: dict, contains all the tensors for input and will be populated
        with more intermeddiate tensors.
      args: arguments that are set for training.

    Returns:
      train_op, summary_op, data_dict, logger_dict, global_step
      train_op: tf op for running training.
      summary_op: tf summary op that needs to be run for populating the
        summaries.
      data_dict: dictionary of tensors. Keys are tensor names and values
        are tensors. New keys and tensors will be added to the input
        data_dict.
      logger_dict: dictionary of tensors for printing.
      global_step: tf.Step that keeps the step number of the training.
    """
    logger_dict = {}
    summary_dict = {}
    global_step = None
    pc = data_dict['{}_pc'.format(scope)]
    gripper_pc_latent = None
    pc_latent = None

    gt_cps = tf_utils.get_control_point_tensor(get_shape(pc)[0])
    ones = tf.ones((get_shape(gt_cps)[0], get_shape(gt_cps)[1], 1), dtype=tf.float32)
    gt_cps = tf.concat((gt_cps, ones), -1) # B x N x 4

    data_dict['{}_gt_cps'.format(scope)] = gt_cps
    if args.gripper_pc_npoints == -1: # Use a pre-defined set of points on the gripper. 5 points. Used in the paper
        grasp_pc_o = gt_cps
    else:
        grasp_pc_o = tf_utils.get_gripper_pc(get_shape(pc)[0], args.gripper_pc_npoints)


    if '{}_grasp_eulers'.format(scope) in data_dict: # Refinement
        assert args.is_training == False
        assert '{}_grasp_translations'.format(scope) in data_dict
        assert isinstance(data_dict['{}_grasp_eulers'.format(scope)], list)
        assert len(data_dict['{}_grasp_eulers'.format(scope)]) == 3

        sample_batch_size = get_shape(pc)[0]
        sample_rotation = data_dict['{}_grasp_eulers'.format(scope)]
        sample_translation = data_dict['{}_grasp_translations'.format(scope)]
        
        verify_tensor_size(pc, [sample_batch_size, npoints if npoints > 0 else args.npoints, 3])
        for i in range(3):
            verify_tensor_size(sample_rotation[i], [sample_batch_size])
        verify_tensor_size(sample_translation, [sample_batch_size, 3])

        rot = tf_utils.tf_rotation_matrix(*sample_rotation, batched=True)
        grasp_pc = tf_utils.get_control_point_tensor(sample_batch_size)
        grasp_pc = tf.matmul(grasp_pc, rot, transpose_a=False, transpose_b=True)
        grasp_pc += tf.expand_dims(sample_translation, 1)
    else: # Training grasp generation
        assert args.is_training

        gt_cps = tf_utils.get_control_point_tensor(get_shape(pc)[0])  # Samples of the 3d points on the gripper
        ones = tf.ones((get_shape(gt_cps)[0], get_shape(gt_cps)[1], 1), dtype=tf.float32)
        gt_cps = tf.concat((gt_cps, ones), -1) # B x N x 4

        data_dict['{}_gt_cps'.format(scope)] = gt_cps
        if args.gripper_pc_npoints == -1: # Use a pre-defined set of points on the gripper. 5 points. Used in the paper
            grasp_pc_o = gt_cps
        else:
            grasp_pc_o = tf_utils.get_gripper_pc(get_shape(pc)[0], args.gripper_pc_npoints)

        grasp_pc = tf.matmul(grasp_pc_o, data_dict['{}_grasp_rt'.format(scope)], transpose_a=False, transpose_b=True) # apply the transformation to the gripper pc
        grasp_pc = tf.slice(grasp_pc, [0, 0, 0], [-1, -1, 3]) # remove last dimension; B x N x 3
        data_dict['{}_grasp_pc'.format(scope)] = grasp_pc
        label = data_dict['{}_label'.format(scope)]
        

    with tf.variable_scope(scope):
        pc_input = tf.slice(pc, [0, 0, 0], [-1, -1, 3])

        success_logit, confidence = models.model.evaluator_model(
            # Confidence of the prediction; Not used now, i.e. confidence==1 (by setting the weight of the confidence loss to a large number)
            pc_input,
            grasp_pc,
            is_training=tf.constant(False), # May be buggy with the batchnorm with evaluator. Disabled.
            # right now the evaluator model does not work with batch norm, and I don't know why. VAE is fine with batch norm.
            bn_decay=None,
            scale=1,
            pc_latent=pc_latent,
            gripper_pc_latent=gripper_pc_latent)
        data_dict['{}_pred/evaluator'.format(scope)] = tf.nn.softmax(success_logit) # Predicted success
        data_dict['{}_pred/confidence'.format(scope)] = confidence

        if args.is_training:
            global_step = tf.train.get_or_create_global_step()
            loss, confidence_term = models.model.classification_with_confidence_loss(success_logit, label, confidence,
                                                                                     args.confidence_weight)
            total_loss = loss + confidence_term

            learning_rate = tf.constant(args.lr, tf.float32)
            if args.ngpus == 1:
                optimizer = tf.train.AdamOptimizer(learning_rate)
            else:
                optimizer = tf.train.AdamOptimizer(learning_rate * hvd.size())
                optimizer = hvd.DistributedOptimizer(optimizer)

            # with tf.control_dependencies(update_ops):
            train_op = optimizer.minimize(total_loss, global_step=global_step, var_list=tf.global_variables())
            confidences = [0.2, 0.4, 0.6, 0.8]
            for c in confidences:
                acc_at_confidence, ratio_at_confidence = models.model.accuracy_better_than_threshold(success_logit,
                                                                                                     label, confidence,
                                                                                                     c)

                summary_dict['ratio_at_each_confidence/' + str(c)] = ratio_at_confidence
                summary_dict['acc_at_Each_confidence/' + str(c)] = acc_at_confidence
            summary_dict['losses/classification_loss'] = loss
            summary_dict['losses/confidence_loss'] = confidence_term
            summary_dict['losses/total_loss'] = total_loss
            summary_dict['step'] = global_step
            logger_dict['predictions'] = tf.math.argmax(success_logit, -1)

            for k in summary_dict:
                logger_dict[k] = summary_dict[k]
                summary_dict[k] = tf.summary.scalar(k, summary_dict[k])

            summary_op = tf.summary.merge(list(summary_dict.values()))
        else:
            train_op = None
            summary_op = None
            logger_dict = None
            tf_success = tf.slice(data_dict['{}_pred/evaluator'.format(scope)], [0, 1], [-1, 1]) # Got the success column
            data_dict['{}_pred/success'.format(scope)] = tf_success
            data_dict['{}_gradient'.format(scope)] = tf.gradients(
                tf_success,
                [data_dict['{}_grasp_translations'.format(scope)], data_dict['{}_grasp_eulers'.format(scope)][0], data_dict['{}_grasp_eulers'.format(scope)][1],
                 data_dict['{}_grasp_eulers'.format(scope)][2]]
            )

        return train_op, summary_op, data_dict, logger_dict, global_step


def build_vae_ops(data_dict, args, scope='vae'):
    """
      builds vae operations that are required for training/inference of vae.

      Args:
        data_dict: dict, contains the tensors for the input to the model.
        args: arguments that are set for training.
        scope: string.
      
      Returns:
        train_op, summary_op, data_dict, logger_dict, global_step
        train_op: tf op for running training.
        summary_op: tf summary op that needs to be run for populating the
            summaries.
        data_dict: dictionary of tensors. Keys are tensor names and values
            are tensors. New keys and tensors will be added to the input
            data_dict.
        logger_dict: dictionary of tensors for printing.
        global_step: tf.Step that keeps the step number of the training.
    """
    losses = None
    summaries = None
    train_op = None
    logger_dict = None
    summary_op = None
    global_step = None
    first_dimension = args.num_objects_per_batch * args.num_grasps_per_object
    is_training = args.is_training

    with tf.variable_scope(scope):
        if is_training:
            assert '{}_pred/samples' not in data_dict
            input_pcs = data_dict['{}_pc'.format(scope)]
            losses = {}
            summaries = {}

            gt_control_points = tf_utils.transform_control_points(data_dict['{}_grasp_rt'.format(scope)], first_dimension, mode='rt')
            gt_control_points = tf.slice(gt_control_points, [0, 0, 0], [-1, -1, 3])
            data_dict['{}_gt_control_point'.format(scope)] = gt_control_points
            pc_input = tf.slice(input_pcs, [0, 0, 0], [-1, -1, 3])

            if not args.gan: # Create Encoder.            
                latent_input = data_dict['{}_grasp_rt'.format(scope)]
                batch_size = get_shape(pc_input)[0]
                npoints = get_shape(pc_input)[1]
                latent_input = tf.tile(tf.reshape(latent_input, [batch_size, 1, -1]), [1, npoints, 1])

                with tf.variable_scope('encoder'):
                    latent_mean_std = models.model.model_with_confidence(
                        pc_input,
                        latent_input,
                        is_training=tf.constant(is_training),
                        bn_decay=None,
                        is_encoder=True,
                        latent_size=args.latent_size,
                        scale=args.model_scale,
                        merge_pcs=args.merge_pcs_in_vae_encoder,
                        pointnet_radius=args.pointnet_radius,
                        pointnet_nclusters=args.pointnet_nclusters
                    )

                    latent_mean = tf.slice(
                        latent_mean_std,
                        [0, 0],
                        [-1, args.latent_size]
                    )
                    latent_std = tf.slice(
                        latent_mean_std,
                        [0, args.latent_size],
                        [-1, args.latent_size]
                    )

                with tf.variable_scope('sample_from_latent'):
                    samples = latent_mean + tf.exp(latent_std / 2.0) * tf.random_normal(
                        latent_mean.shape, 0, 1, dtype=tf.float32)
                    data_dict['{}_pred/samples'.format(scope)] = samples

                kl_loss = models.model.kl_divergence(latent_mean, latent_std)
                kl_loss = tf.reduce_mean(kl_loss)
                losses['kl_loss'] = kl_loss * args.kl_loss_weight
                summaries['unscaled_kl_loss'] = kl_loss
            else: # For gan just sample random latents.
                samples = tf.random.uniform(
                    [first_dimension, args.latent_size],
                    name='gan_latents'
                )
        else:
            input_pcs = data_dict['{}_pc'.format(scope)]
            samples = data_dict['{}_pred/samples'.format(scope)]

        with tf.variable_scope('decoder'):
            pc_input = tf.slice(input_pcs, [0, 0, 0], [-1, -1, 3])

            latent_input = samples
            batch_size = get_shape(pc_input)[0]
            npoints = get_shape(pc_input)[1]
            latent_input = tf.tile(tf.reshape(latent_input, [batch_size, 1, -1]), [1, npoints, 1])

            q, t, confidence = models.model.model_with_confidence(
                pc_input,
                latent_input,
                tf.constant(is_training),
                bn_decay=None,
                is_encoder=False,
                latent_size=None,
                scale=args.model_scale,
                pointnet_radius=args.pointnet_radius,
                pointnet_nclusters=args.pointnet_nclusters
            )
            predicted_qt = tf.concat((q, t), -1)
            data_dict['{}_pred/grasp_qt'.format(scope)] = predicted_qt
            data_dict['{}_pred/confidence'.format(scope)] = confidence

            cp = tf_utils.transform_control_points(predicted_qt,
                                                   get_shape(data_dict['{}_pc'.format(scope)])[0],
                                                   scope='transform_predicted_qt')
            data_dict['{}_pred/cps'.format(scope)] = cp

        if is_training:
            loss_fn = None
            if args.gan:
                loss_fn = models.model.min_distance_loss
            else:
                loss_fn = models.model.control_point_l1_loss

            loss_term, confidence_term = loss_fn(
                cp,
                gt_control_points,
                confidence=confidence,
                confidence_weight=args.confidence_weight
            )
            data_dict['{}_loss'.format(scope)] = loss_term
            losses['gan_min_dist' if args.gan else 'L1_grasp_reconstruction'] = loss_term
            losses['confidence'] = confidence_term

            for c in CONFIDENCES:
                qkey = 'quality_at_confidence/{}'.format(c)
                rkey = 'ratio_at_confidence/{}'.format(c)
                summary_fn = models.model.control_point_l1_loss_better_than_threshold
                if args.gan:
                    summary_fn = models.model.min_distance_better_than_threshold
                summaries[qkey], summaries[rkey] = summary_fn(
                    cp,
                    gt_control_points,
                    confidence,
                    c
                )

            global_step = tf.train.get_or_create_global_step()
            total_loss = tf.reduce_sum(tf.stack(list(losses.values())))
            summaries['total_loss'] = total_loss
            learning_rate = tf.constant(args.lr, dtype=tf.float32)

            if args.ngpus > 1:
                optimizer = tf.train.AdamOptimizer(learning_rate * hvd.size())
                optimizer = hvd.DistributedOptimizer(optimizer)
            else:
                optimizer = tf.train.AdamOptimizer(learning_rate)

            train_op = optimizer.minimize(total_loss, global_step=global_step)
            summaries['global_step'] = global_step
            for k in losses:
                summaries['loss/{}'.format(k)] = losses[k]

            logger_dict = {}
            for k, v in summaries.items():
                logger_dict[k] = summaries[k]
                summaries[k] = tf.summary.scalar(k, v)

            summary_op = tf.summary.merge(list(summaries.values()))

        return train_op, summary_op, data_dict, logger_dict, global_step


def build_tf_ops(args, files, data_dict):
    """Builds tf ops for evaluator model and vae."""
    pcreader = None

    if args.is_training:
        pcreader = grasp_data_reader.PointCloudReader(
            root_folder=args.dataset_root_folder,
            batch_size=args.num_grasps_per_object,
            num_grasp_clusters=args.num_grasp_clusters,
            npoints=args.npoints,
            min_difference_allowed=(0, 0, 0),
            max_difference_allowed=(3, 3, 0),
            occlusion_nclusters=0,
            occlusion_dropout_rate=0.,
            use_uniform_quaternions=args.use_uniform_quaternions,
            ratio_of_grasps_used=args.grasps_ratio,
        )

    first_dimension = args.num_objects_per_batch * args.num_grasps_per_object

    if args.train_evaluator:
        if data_dict is None:
            data_dict = get_evaluator_data_dict(first_dimension, args, files, pcreader)
        return build_evaluator_ops(data_dict, args), pcreader
    else:
        if data_dict is None:
            data_dict = get_vae_data_dict(first_dimension, args, files, pcreader)
        return build_vae_ops(data_dict, args), pcreader


def main():
    parser = make_parser()
    args = parser.parse_args(sys.argv[1:])

    if args.model_scale < 1:
        raise ValueError('model_scale should be >= 1.')

    if args.seed != -1:
        set_seed(args.seed)

    config = tf.ConfigProto()
    config.gpu_options.allow_growth = True
    if args.ngpus > 1:
        hvd.init()
        config.gpu_options.visible_device_list = str(hvd.local_rank())
    else:
        os.environ["CUDA_DEVICE_ORDER"] = "PCI_BUS_ID"
        os.environ["CUDA_VISIBLE_DEVICES"] = args.gpu
    
    if args.is_training:

        files = get_files(args.dataset_root_folder, args.allowed_categories, args.blacklisted_categories, args.training_splits, args.splits_folder_name, args.grasps_folder_name)

        random.shuffle(files)
        print('files ====>', files)
        print(len(files))

        main_train(args, files, config)
    else:
        raise NotImplementedError('This is the train script. is_training should be 1')


def main_train(args, files, tf_config):
    assert args.logdir != '', 'logdir cannot be empty'
    logdir = os.path.join(args.logdir, 'tf_output')

    if os.path.isdir(logdir):
        do_not_delete = True
        if args.ngpus > 1:
            if hvd.rank() == 0:
                if args.force_continue:
                    do_not_delete = True
                else:
                    do_not_delete = False
            else:
                do_not_delete = True

        elif HEADLESS:
            if args.force_continue:
                do_not_delete = True
            else:
                raise ValueError('{} exists'.format(logdir))
        else:
            while True:
                try:
                    key = input('{} \n do you want to continue?'.format(logdir))
                except NameError:
                    key = 'y'
                if key == 'y':
                    break
                elif key == 'n':
                    do_not_delete = False
                    break
                else:
                    print('invalid key')

        if not do_not_delete:
            print('******* Deleting {} *******'.format(logdir))
            os.system('rm -r {}'.format(logdir))
        else:
            print('continuing')
    elif args.ngpus == 1 or hvd.rank() == 0:
        os.makedirs(logdir)

    print('logdir is {}'.format(logdir))

    tf_output, pc_reader = build_tf_ops(
        args=args, data_dict=None, files=files,
    )

    train_op, summary_op, tf_data_dict, logger_dict, tf_step = tf_output

    summary_hook = tf.train.SummarySaverHook(
        summary_op=summary_op,
        output_dir=logdir,
        save_steps=args.save_steps,
    )

    logging_hook = tf.train.LoggingTensorHook(
        tensors=logger_dict,
        every_n_iter=args.log_steps,
    )

    hooks = []
    if args.ngpus > 1:
        hooks.append(hvd.BroadcastGlobalVariablesHook(0))
        if hvd.rank() == 0:
            checkpoint_dir = logdir
            save_checkpoint_secs = 300
            hooks += [logging_hook, summary_hook]
        else:
            checkpoint_dir = None
            save_checkpoint_secs = 0
            hooks += [logging_hook]
    else:
        hooks = [logging_hook, summary_hook]
        checkpoint_dir = logdir
        save_checkpoint_secs = 300

    if args.init_checkpoint_folder != '':
        checkpoint = tf.train.latest_checkpoint(args.init_checkpoint_folder)
        tf.train.init_from_checkpoint(checkpoint, {'/': '/'})

    
    yaml_path = os.path.join(args.logdir, 'args.yaml')

    with open(yaml_path, 'w') as yaml_file:
        yaml.dump(args, yaml_file)

    with tf.train.MonitoredTrainingSession(
      checkpoint_dir=checkpoint_dir,
      hooks=hooks,
      save_summaries_secs=0,
      save_checkpoint_secs=save_checkpoint_secs,
      config=tf_config,
    ) as mon_sess:
        start_time = time.time()
        print(time.time() - start_time)
        writer = SummaryWriterCache.get(logdir)

        while not mon_sess.should_stop():
            # print('hvd rank = {}, current_index = {}, nfiles = {}'.format(current_index, hvd.rank(), len(my_files)))
            tensor_list = [tf_step, tf_data_dict]
            if args.training_splits == 'train':
                tensor_list += [train_op] + tensor_list
            # _, step, data_dict = mon_sess.run(tensor_list)
            mon_sess.run(tensor_list)


def copy_weights(source_scope, destination_scope):
    # Copies tensorflow variables from source_scope to destination_scope.
    source_variables = {v.name[len(source_scope) + 1:]: v for v in tf.global_variables(source_scope)}
    destination_variables = {v.name[len(destination_scope) + 1:]: v for v in tf.global_variables(destination_scope)}
    assert (set(source_variables.keys()) == set(destination_variables.keys())), '{} {}'.format(source_scope,
                                                                                               destination_scope)

    copy_ops = [tf.assign(destination_variables[s], source_variables[s]) for s in source_variables]
    return copy_ops


def normalize_pc_size(input_pc, size):
    index = np.random.choice(range(input_pc.shape[0]), size=size, replace=input_pc.shape[0] < size)
    return input_pc[index, :]

def normalize_pc_size_selection(input_pc, size):
    selection = np.random.choice(range(input_pc.shape[0]), size=size, replace=input_pc.shape[0] < size)
    return input_pc[selection, :], selection

if __name__ == '__main__':
    main()
