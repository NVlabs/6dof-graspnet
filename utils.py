# Copyright (c) 2019, NVIDIA CORPORATION.  All rights reserved.
#
# NVIDIA CORPORATION and its licensors retain all intellectual property
# and proprietary rights in and to this software, related documentation
# and any modifications thereto.  Any use, reproduction, disclosure or
# distribution of this software and related documentation without an express
# license agreement from NVIDIA CORPORATION is strictly prohibited.
import argparse
import os
import json
import random

import sample
import tensorflow as tf
import numpy as np

def get_files(root_dir, allowed_categories, blacklisted_categories, training_splits, splits_folder_name='splits', grasps_folder_name='grasps'):
    split_files = os.listdir(os.path.join(root_dir, splits_folder_name))
    files = []
    for split_file in split_files:
        if split_file.find('.json') < 0:
            continue

        should_go_through = False
        if allowed_categories == '':
            should_go_through = True
            if blacklisted_categories != '':
                if blacklisted_categories.find(split_file[:-5]) >= 0:
                    should_go_through = False
        else:
            if allowed_categories.find(split_file[:-5]) >= 0:
                should_go_through = True

        if should_go_through:
            files += [os.path.join(root_dir, grasps_folder_name, f) for f in
                        json.load(open(os.path.join(root_dir, splits_folder_name, split_file)))[training_splits]]
    return files

def set_seed(seed):
    random.seed(seed)
    tf.random.set_random_seed(seed)
    np.random.seed(seed)

def load_object(path, scale):
    object_model = sample.Object(path)
    object_model.rescale(scale)
    object_model = object_model.mesh
    object_model.vertices -= np.mean(object_model.vertices, 0, keepdims=1)
    return object_model

def mkdir(path):
    if not os.path.isdir(path):
        os.makedirs(path)

def make_parser():
    parser = argparse.ArgumentParser(description='6 DOF GraspNet parser.',
                                     formatter_class=argparse.ArgumentDefaultsHelpFormatter)

    parser.add_argument('--num_objects_per_batch', type=int, default=1, help='data batch size.')
    parser.add_argument('--num_grasps_per_object', type=int, default=64)
    parser.add_argument('--npoints', type=int, default=1024, help='number of points in each batch')
    parser.add_argument('--occlusion_nclusters', type=int, default=0,
                        help='clusters the points to nclusters to be selected for simulating the dropout')
    parser.add_argument('--occlusion_dropout_rate', type=float, default=0,
                        help='probability at which the clusters are removed from point cloud.')
    parser.add_argument('--depth_noise', type=float, default=0.0)  # to be used in the data reader.
    parser.add_argument('--dataset_root_folder', type=str, default='unified_grasp_data/',
                        help='path to root directory of the dataset.')
    parser.add_argument('--kl_loss_weight', type=float, default=0.01)
    parser.add_argument('--lr', type=float, default=0.0001)
    parser.add_argument('--num_grasp_clusters', type=int, default=32)
    parser.add_argument('--gpu', type=str, default='0')
    parser.add_argument('--solver', type=str, default='adam')
    parser.add_argument('--logdir', type=str, default='')
    parser.add_argument('--force_continue', type=int, default=0)
    parser.add_argument('--training_splits', type=str, default='train',
                        help='can be any combination of train and test without any space.')
    parser.add_argument('--is_training', type=int, default=1)
    parser.add_argument('--gripper', type=str, default='panda',
                        help='type of the gripper. Leave it to panda if you want to use it for franka robot')
    parser.add_argument('--latent_size', type=int, default=2)
    parser.add_argument('--confidence_weight', type=float, default=1.0,
                        help='initially I wanted to compute confidence for vae and evaluator outputs, '
                             'setting the confidence weight to 1. immediately pushes the confidence to 1.0.')
    parser.add_argument('--train_evaluator', type=int, default=0,
                        help='if set to 1, trains evaluator. 0: trains vae')
    parser.add_argument('--gripper_pc_npoints', type=int, default=-1,
                        help='number of points representing the gripper. -1 just uses the points on the finger and also the base. other values use subsampling of the gripper mesh')
    parser.add_argument('--merge_pcs_in_vae_encoder', type=int, default=0,
                        help='whether to create unified pc in encoder by coloring the points (similar to evaluator')
    parser.add_argument('--ngpus', type=int, default=1, help='number of gpus used')
    parser.add_argument('--init_checkpoint_folder', type=str, default='',
                        help='if set to non-empty string, it initializes from the given checkpoint path')
    parser.add_argument('--allowed_categories', type=str, default='',
                        help='if left blank uses all the categories in the <DATASET_ROOT_PATH>/splits/<category>.json, otherwise only chooses the categories that are set.')
    parser.add_argument('--blacklisted_categories', type=str, default='',
                        help='The opposite of allowed categories')
    parser.add_argument('--seed', help='Number of objects to render in scene', type=int, default=-1)
    parser.add_argument('--save_steps', help='How often to save logs and metrics', type=int, default=50)
    parser.add_argument('--log_steps', help='How often to print', type=int, default=50)
    parser.add_argument('--use_uniform_quaternions', type=int, default=0)
    parser.add_argument('--grasps_ratio', type=float, default=1.0,
                        help='used for checkng the effect of number of grasps per object on the success of the model.')
    parser.add_argument('--model_scale', type=int, default=1,
                        help='the scale of the parameters. Use scale >= 1. Scale=2 increases the number of parameters in model by 4x.')
    parser.add_argument('--splits_folder_name', type=str, default='splits',
                        help='Folder name for the directory that has all the jsons for train/test splits.')
    parser.add_argument('--grasps_folder_name', type=str, default='grasps',
                        help='Directory that contains the grasps. Will be joined with the dataset_root_folder and the file names as defined in the splits.')
    parser.add_argument('--pointnet_radius', help='Radius for ball query for PointNet++, just the first layer', type=float, default=0.02)
    parser.add_argument('--pointnet_nclusters', help='Number of cluster centroids for PointNet++, just the first layer', type=int, default=128)
    parser.add_argument('--gan', type=int, default=0, help='If 1 uses gan formulation to train instead of vae')

   
    return parser

def transform_control_points_numpy(gt_grasps, batch_size):
    grasp_shape = gt_grasps.shape
    assert(len(grasp_shape) == 3), grasp_shape
    assert(grasp_shape[1] == 4 and grasp_shape[2] == 4), grasp_shape

    control_points = get_control_points(batch_size)
    shape = control_points.shape
    ones = np.ones((shape[0], shape[1], 1))
    control_points = np.concatenate((control_points, ones), -1)
    # return tf.matmul(control_points, gt_grasps, transpose_a=False, transpose_b=True)

    for i in range(batch_size):
        control_points[i, :, :] = control_points[i, :, :].dot(gt_grasps[i, :, :].T)
    return control_points

def get_control_points(batch_size):
    control_points = np.load('gripper_control_points/panda.npy')[:, :3]
    control_points = [[0, 0, 0], [0, 0, 0], control_points[0, :], control_points[1, :], control_points[-2, :],
                      control_points[-1, :]]
    control_points = np.asarray(control_points, dtype=np.float32)
    control_points = np.tile(np.expand_dims(control_points, 0), [batch_size, 1, 1])
    return control_points

def get_clutter_object_ids(args):
    return [1] + list(range(3, args.n_objects+3))

def convert_to_pc(depth_cv, intrinsic_matrix, return_finite_depth=True):

    depth = depth_cv.astype(np.float32, copy=True)

    # get intrinsic matrix
    K = intrinsic_matrix
    Kinv = np.linalg.inv(K)

    # compute the 3D points
    width = depth.shape[1]
    height = depth.shape[0]

    # construct the 2D points matrix
    x, y = np.meshgrid(np.arange(width), np.arange(height))
    ones = np.ones((height, width), dtype=np.float32)
    x2d = np.stack((x, y, ones), axis=2).reshape(width*height, 3)

    # backprojection
    R = np.dot(Kinv, x2d.transpose())

    # compute the 3D points
    X = np.multiply(np.tile(depth.reshape(1, width*height), (3, 1)), R)
    X = np.array(X).transpose()

    selection = None
    if return_finite_depth:
        selection = np.isfinite(X[:, 0])
        X = X[selection, :]
    


    return X, selection