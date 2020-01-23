# Copyright (c) 2019, NVIDIA CORPORATION.  All rights reserved.
#
# NVIDIA CORPORATION and its licensors retain all intellectual property
# and proprietary rights in and to this software, related documentation
# and any modifications thereto.  Any use, reproduction, disclosure or
# distribution of this software and related documentation without an express
# license agreement from NVIDIA CORPORATION is strictly prohibited.
try:
    import mayavi.mlab as mlab
    from visualization_utils import draw_scene
    import matplotlib.pyplot as plt
except:
    pass
import numpy as np
from grasp_data_reader import regularize_pc_point_count
import trimesh.transformations as tra
import trimesh


def cov_matrix(center, points):
    if points.shape[0] == 0:
        return None
    n = points.shape[0]
    diff = points - np.expand_dims(center, 0)
    cov = diff.T.dot(diff) / diff.shape[0]
    cov /= n

    eigen_values, eigen_vectors = np.linalg.eig(cov)

    order = np.argsort(-eigen_values)

    return eigen_values[order], eigen_vectors[:, order]


def choose_direction(direction, point):
    dot = np.sum(direction * point)
    if dot >= 0:
        return -direction
    return direction


def propose_grasps(pc, radius, num_grasps=1, vis=False):
    output_grasps = []

    for _ in range(num_grasps):
        center_index = np.random.randint(pc.shape[0])
        center_point = pc[center_index, :].copy()
        d = np.sqrt(np.sum(np.square(pc - np.expand_dims(center_point, 0)), -1))
        index = np.where(d < radius)[0]
        neighbors = pc[index, :]
        
        eigen_values, eigen_vectors = cov_matrix(center_point, neighbors)
        direction = eigen_vectors[:, 2]

        direction = choose_direction(direction, center_point)
        
        surface_orientation = trimesh.geometry.align_vectors([0,0,1], direction)
        roll_orientation = tra.quaternion_matrix(
            tra.quaternion_about_axis(np.random.uniform(0, 2*np.pi), [0, 0, 1])
        )
        gripper_transform = surface_orientation.dot(roll_orientation)
        gripper_transform[:3, 3] = center_point


        translation_transform = np.eye(4)
        translation_transform[2, 3] = -np.random.uniform(0.0669, 0.1122)

        gripper_transform = gripper_transform.dot(translation_transform)
        output_grasps.append(gripper_transform.copy())


    if vis:
        draw_scene(pc, grasps=output_grasps)
        mlab.show()
    
    return np.asarray(output_grasps)

