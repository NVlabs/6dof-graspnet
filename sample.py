# Copyright (c) 2019, NVIDIA CORPORATION.  All rights reserved.
#
# NVIDIA CORPORATION and its licensors retain all intellectual property
# and proprietary rights in and to this software, related documentation
# and any modifications thereto.  Any use, reproduction, disclosure or
# distribution of this software and related documentation without an express
# license agreement from NVIDIA CORPORATION is strictly prohibited.
# -*- coding: utf-8 -*-
"""Helper classes and functions to sample grasps for a given object mesh."""

from __future__ import print_function

import argparse
from collections import OrderedDict
import errno
import json
import os
import numpy as np

from tqdm import tqdm

import trimesh
import trimesh.transformations as tra


class Object(object):
    """Represents a graspable object."""

    def __init__(self, filename):
        """Constructor.

        :param filename: Mesh to load
        :param scale: Scaling factor
        """
        self.mesh = trimesh.load(filename)
        self.scale = 1.0

        # print(filename)
        self.filename = filename
        if isinstance(self.mesh, list):
            # this is fixed in a newer trimesh version:
            # https://github.com/mikedh/trimesh/issues/69
            print("Warning: Will do a concatenation")
            self.mesh = trimesh.util.concatenate(self.mesh)

        self.collision_manager = trimesh.collision.CollisionManager()
        self.collision_manager.add_object('object', self.mesh)

    def rescale(self, scale=1.0):
        """Set scale of object mesh.

        :param scale
        """
        self.scale = scale
        self.mesh.apply_scale(self.scale)

    def resize(self, size=1.0):
        """Set longest of all three lengths in Cartesian space.

        :param size
        """
        self.scale = size / np.max(self.mesh.extents)
        self.mesh.apply_scale(self.scale)

    def in_collision_with(self, mesh, transform):
        """Check whether the object is in collision with the provided mesh.

        :param mesh:
        :param transform:
        :return: boolean value
        """
        return self.collision_manager.in_collision_single(mesh, transform=transform)


class PandaGripper(object):
    """An object representing a Franka Panda gripper."""

    def __init__(self, q=None, num_contact_points_per_finger=10, root_folder=''):
        """Create a Franka Panda parallel-yaw gripper object.

        Keyword Arguments:
            q {list of int} -- configuration (default: {None})
            num_contact_points_per_finger {int} -- contact points per finger (default: {10})
            root_folder {str} -- base folder for model files (default: {''})
        """
        self.joint_limits = [0.0, 0.04]
        self.default_pregrasp_configuration = 0.04

        if q is None:
            q = self.default_pregrasp_configuration

        self.q = q
        fn_base = root_folder + 'gripper_models/panda_gripper/hand.stl'
        fn_finger = root_folder + 'gripper_models/panda_gripper/finger.stl'
        self.base = trimesh.load(fn_base)
        self.finger_l = trimesh.load(fn_finger)
        self.finger_r = self.finger_l.copy()

        # transform fingers relative to the base
        self.finger_l.apply_transform(tra.euler_matrix(0, 0, np.pi))
        self.finger_l.apply_translation([+q, 0, 0.0584])
        self.finger_r.apply_translation([-q, 0, 0.0584])

        self.fingers = trimesh.util.concatenate([self.finger_l, self.finger_r])
        self.hand = trimesh.util.concatenate([self.fingers, self.base])

        self.ray_origins = []
        self.ray_directions = []
        for i in np.linspace(-0.01, 0.02, num_contact_points_per_finger):
            self.ray_origins.append(
                np.r_[self.finger_l.bounding_box.centroid + [0, 0, i], 1])
            self.ray_origins.append(
                np.r_[self.finger_r.bounding_box.centroid + [0, 0, i], 1])
            self.ray_directions.append(
                np.r_[-self.finger_l.bounding_box.primitive.transform[:3, 0]])
            self.ray_directions.append(
                np.r_[+self.finger_r.bounding_box.primitive.transform[:3, 0]])

        self.ray_origins = np.array(self.ray_origins)
        self.ray_directions = np.array(self.ray_directions)

        self.standoff_range = np.array([max(self.finger_l.bounding_box.bounds[0, 2],
                                            self.base.bounding_box.bounds[1, 2]),
                                        self.finger_l.bounding_box.bounds[1, 2]])
        self.standoff_range[0] += 0.001

    def get_obbs(self):
        """Get list of obstacle meshes.

        Returns:
            list of trimesh -- bounding boxes used for collision checking
        """
        return [self.finger_l.bounding_box, self.finger_r.bounding_box, self.base.bounding_box]

    def get_meshes(self):
        """Get list of meshes that this gripper consists of.

        Returns:
            list of trimesh -- visual meshes
        """
        return [self.finger_l, self.finger_r, self.base]

    def get_closing_rays(self, transform):
        """Get an array of rays defining the contact locations and directions on the hand.

        Arguments:
            transform {[nump.array]} -- a 4x4 homogeneous matrix

        Returns:
            numpy.array -- transformed rays (origin and direction)
        """
        return transform[:3, :].dot(
            self.ray_origins.T).T, transform[:3, :3].dot(self.ray_directions.T).T


def get_available_grippers():
    """Get list of names of all available grippers.

    Returns:
        list of str -- a list of names for the gripper factory
    """
    available_grippers = OrderedDict({
        'panda': PandaGripper,
    })
    return available_grippers


def create_gripper(name, configuration=None, root_folder=''):
    """Create a gripper object.

    Arguments:
        name {str} -- name of the gripper

    Keyword Arguments:
        configuration {list of float} -- configuration (default: {None})
        root_folder {str} -- base folder for model files (default: {''})

    Raises:
        Exception: If the gripper name is unknown.

    Returns:
        [type] -- gripper object
    """
    if name.lower() == 'panda':
        return PandaGripper(q=configuration, root_folder=root_folder)
    else:
        raise Exception("Unknown gripper: {}".format(name))


def in_collision_with_gripper(object_mesh, gripper_transforms, gripper_name, silent=False):
    """Check collision of object with gripper.

    Arguments:
        object_mesh {trimesh} -- mesh of object
        gripper_transforms {list of numpy.array} -- homogeneous matrices of gripper
        gripper_name {str} -- name of gripper

    Keyword Arguments:
        silent {bool} -- verbosity (default: {False})

    Returns:
        [list of bool] -- Which gripper poses are in collision with object mesh
    """
    manager = trimesh.collision.CollisionManager()
    manager.add_object('object', object_mesh)
    gripper_meshes = [create_gripper(gripper_name).hand]
    min_distance = []
    for tf in tqdm(gripper_transforms, disable=silent):
        min_distance.append(np.min([manager.min_distance_single(
            gripper_mesh, transform=tf) for gripper_mesh in gripper_meshes]))

    return [d == 0 for d in min_distance], min_distance


def grasp_quality_point_contacts(transforms, collisions, object_mesh, gripper_name='panda', silent=False):
    """Grasp quality function

    Arguments:
        transforms {[type]} -- grasp poses
        collisions {[type]} -- collision information
        object_mesh {trimesh} -- object mesh

    Keyword Arguments:
        gripper_name {str} -- name of gripper (default: {'panda'})
        silent {bool} -- verbosity (default: {False})

    Returns:
        list of float -- quality of grasps [0..1]
    """
    res = []
    gripper = create_gripper(gripper_name)
    if trimesh.ray.has_embree:
        intersector = trimesh.ray.ray_pyembree.RayMeshIntersector(
            object_mesh, scale_to_box=True)
    else:
        intersector = trimesh.ray.ray_triangle.RayMeshIntersector(object_mesh)
    for p, colliding in tqdm(zip(transforms, collisions), total=len(transforms), disable=silent):
        if colliding:
            res.append(-1)
        else:
            ray_origins, ray_directions = gripper.get_closing_rays(p)
            locations, index_ray, index_tri = intersector.intersects_location(
                ray_origins, ray_directions, multiple_hits=False)

            if len(locations) == 0:
                res.append(0)
            else:
                # this depends on the width of the gripper
                valid_locations = np.linalg.norm(
                    ray_origins[index_ray]-locations, axis=1) < 2.0*gripper.q

                if sum(valid_locations) == 0:
                    res.append(0)
                else:
                    contact_normals = object_mesh.face_normals[index_tri[valid_locations]]
                    motion_normals = ray_directions[index_ray[valid_locations]]
                    dot_prods = (motion_normals * contact_normals).sum(axis=1)
                    res.append(np.cos(dot_prods).sum() / len(ray_origins))
    return res


def grasp_quality_antipodal(transforms, collisions, object_mesh, gripper_name='panda', silent=False):
    """Grasp quality function.

    Arguments:
        transforms {numpy.array} -- grasps
        collisions {list of bool} -- collision information
        object_mesh {trimesh} -- object mesh

    Keyword Arguments:
        gripper_name {str} -- name of gripper (default: {'panda'})
        silent {bool} -- verbosity (default: {False})

    Returns:
        list of float -- quality of grasps [0..1]
    """
    res = []
    gripper = create_gripper(gripper_name)
    if trimesh.ray.has_embree:
        intersector = trimesh.ray.ray_pyembree.RayMeshIntersector(
            object_mesh, scale_to_box=True)
    else:
        intersector = trimesh.ray.ray_triangle.RayMeshIntersector(object_mesh)
    for p, colliding in tqdm(zip(transforms, collisions), total=len(transforms), disable=silent):
        if colliding:
            res.append(0)
        else:
            ray_origins, ray_directions = gripper.get_closing_rays(p)
            locations, index_ray, index_tri = intersector.intersects_location(
                ray_origins, ray_directions, multiple_hits=False)

            if locations.size == 0:
                res.append(0)
            else:
                # chose contact points for each finger [they are stored in an alternating fashion]
                index_ray_left = np.array([i for i, num in enumerate(
                    index_ray) if num % 2 == 0 and np.linalg.norm(ray_origins[num]-locations[i]) < 2.0*gripper.q])
                index_ray_right = np.array([i for i, num in enumerate(
                    index_ray) if num % 2 == 1 and np.linalg.norm(ray_origins[num]-locations[i]) < 2.0*gripper.q])

                if index_ray_left.size == 0 or index_ray_right.size == 0:
                    res.append(0)
                else:
                    # select the contact point closest to the finger (which would be hit first during closing)
                    left_contact_idx = np.linalg.norm(
                        ray_origins[index_ray[index_ray_left]] - locations[index_ray_left], axis=1).argmin()
                    right_contact_idx = np.linalg.norm(
                        ray_origins[index_ray[index_ray_right]] - locations[index_ray_right], axis=1).argmin()
                    left_contact_point = locations[index_ray_left[left_contact_idx]]
                    right_contact_point = locations[index_ray_right[right_contact_idx]]

                    left_contact_normal = object_mesh.face_normals[index_tri[index_ray_left[left_contact_idx]]]
                    right_contact_normal = object_mesh.face_normals[
                        index_tri[index_ray_right[right_contact_idx]]]

                    l_to_r = (right_contact_point - left_contact_point) / \
                        np.linalg.norm(right_contact_point -
                                       left_contact_point)
                    r_to_l = (left_contact_point - right_contact_point) / \
                        np.linalg.norm(left_contact_point -
                                       right_contact_point)

                    qual_left = np.dot(left_contact_normal, r_to_l)
                    qual_right = np.dot(right_contact_normal, l_to_r)
                    if qual_left < 0 or qual_right < 0:
                        qual = 0
                    else:
                        # qual = qual_left * qual_right
                        qual = min(qual_left, qual_right)
                    # math.cos(math.atan(friction_coefficient))

                    res.append(qual)
    return res


def raycast_collisioncheck(origins, expected_hit_points, object_mesh):
    """ Check whether a set of ray casts turn out as expected.

    :param origins: ray origins and directions as Nx4x4 homogenous matrices (use last two columns)
    :param expected_hit_points: 3d points Nx3
    :param object_mesh: trimesh mesh instance

    :return: boolean array of size N
    """
    assert len(origins) == len(expected_hit_points)

    if trimesh.ray.has_embree:
        intersector = trimesh.ray.ray_pyembree.RayMeshIntersector(
            object_mesh, scale_to_box=True)
    else:
        intersector = trimesh.ray.ray_triangle.RayMeshIntersector(object_mesh)

    locations, index_rays, _ = intersector.intersects_location(
        origins[:, :3, 3], origins[:, :3, 2], multiple_hits=False)
    res = np.array([False] * len(origins))
    res[index_rays] = np.all(np.isclose(
        locations, expected_hit_points[index_rays]), axis=1)

    return res


def sample_multiple_grasps(number_of_candidates, mesh, gripper_name, systematic_sampling,
                           surface_density=0.005*0.005, standoff_density=0.01, roll_density=15,
                           type_of_quality='antipodal',
                           min_quality=-1.0,
                           silent=False):
    """Sample a set of grasps for an object.

    Arguments:
        number_of_candidates {int} -- Number of grasps to sample
        mesh {trimesh} -- Object mesh
        gripper_name {str} -- Name of gripper model
        systematic_sampling {bool} -- Whether to use grid sampling for roll

    Keyword Arguments:
        surface_density {float} -- surface density, in m^2 (default: {0.005*0.005})
        standoff_density {float} -- density for standoff, in m (default: {0.01})
        roll_density {float} -- roll density, in deg (default: {15})
        type_of_quality {str} -- quality metric (default: {'antipodal'})
        min_quality {float} -- minimum grasp quality (default: {-1})
        silent {bool} -- verbosity (default: {False})

    Raises:
        Exception: Unknown quality metric

    Returns:
        [type] -- points, normals, transforms, roll_angles, standoffs, collisions, quality
    """
    origins = []
    orientations = []
    transforms = []

    standoffs = []
    roll_angles = []

    gripper = create_gripper(gripper_name)

    if systematic_sampling:
        # systematic sampling. input:
        # - Surface density:
        # - Standoff density:
        # - Rotation density:
        # Resulting number of samples:
        # (Area/Surface Density) * (Finger length/Standoff density) * (360/Rotation Density)
        surface_samples = int(np.ceil(mesh.area / surface_density))
        standoff_samples = np.linspace(gripper.standoff_range[0], gripper.standoff_range[1], max(
            1, (gripper.standoff_range[1] - gripper.standoff_range[0]) / standoff_density))
        rotation_samples = np.arange(0, 1 * np.pi, np.deg2rad(roll_density))

        number_of_candidates = surface_samples * \
            len(standoff_samples) * len(rotation_samples)

        tmp_points, face_indices = mesh.sample(
            surface_samples, return_index=True)
        tmp_normals = mesh.face_normals[face_indices]

        number_of_candidates = len(tmp_points) * \
            len(standoff_samples) * len(rotation_samples)
        print("Number of samples ", number_of_candidates, "(", len(tmp_points), " x ", len(standoff_samples), " x ",
              len(rotation_samples), ")")

        points = []
        normals = []

        position_idx = []

        pos_cnt = 0
        cnt = 0

        batch_position_idx = []
        batch_points = []
        batch_normals = []
        batch_roll_angles = []
        batch_standoffs = []
        batch_transforms = []

        for point, normal in tqdm(zip(tmp_points, tmp_normals), total=len(tmp_points), disable=silent):
            for roll in rotation_samples:
                for standoff in standoff_samples:
                    batch_position_idx.append(pos_cnt)
                    batch_points.append(point)
                    batch_normals.append(normal)
                    batch_roll_angles.append(roll)
                    batch_standoffs.append(standoff)

                    orientation = tra.quaternion_matrix(
                        tra.quaternion_about_axis(roll, [0, 0, 1]))
                    origin = point + normal * standoff

                    batch_transforms.append(
                        np.dot(np.dot(tra.translation_matrix(origin), trimesh.geometry.align_vectors([0, 0, -1], normal)),
                               orientation))

                    cnt += 1
            pos_cnt += 1

            if cnt % 1000 == 0 or cnt == len(tmp_points):
                valid = raycast_collisioncheck(np.asarray(
                    batch_transforms), np.asarray(batch_points), mesh)
                transforms.extend(np.array(batch_transforms)[valid])
                position_idx.extend(np.array(batch_position_idx)[valid])
                points.extend(np.array(batch_points)[valid])
                normals.extend(np.array(batch_normals)[valid])
                roll_angles.extend(np.array(batch_roll_angles)[valid])
                standoffs.extend(np.array(batch_standoffs)[valid])

                batch_position_idx = []
                batch_points = []
                batch_normals = []
                batch_roll_angles = []
                batch_standoffs = []
                batch_transforms = []

        points = np.array(points)
        normals = np.array(normals)
        position_idx = np.array(position_idx)
    else:
        points, face_indices = mesh.sample(
            number_of_candidates, return_index=True)
        normals = mesh.face_normals[face_indices]

        # generate transformations
        for point, normal in tqdm(zip(points, normals), total=len(points), disable=silent):
            # roll along approach vector
            angle = np.random.rand() * 2 * np.pi
            roll_angles.append(angle)
            orientations.append(tra.quaternion_matrix(
                tra.quaternion_about_axis(angle, [0, 0, 1])))

            # standoff from surface
            standoff = (gripper.standoff_range[1] - gripper.standoff_range[0]) * np.random.rand() \
                + gripper.standoff_range[0]
            standoffs.append(standoff)
            origins.append(point + normal * standoff)

            transforms.append(
                np.dot(np.dot(tra.translation_matrix(origins[-1]),
                              trimesh.geometry.align_vectors([0, 0, -1], normal)),
                       orientations[-1]))

    verboseprint("Checking collisions...")
    collisions = in_collision_with_gripper(
        mesh, transforms, gripper_name=gripper_name, silent=silent)

    verboseprint("Labelling grasps...")
    quality = {}
    quality_key = 'quality_' + type_of_quality
    if type_of_quality == 'antipodal':
        quality[quality_key] = grasp_quality_antipodal(
            transforms, collisions, object_mesh=mesh, gripper_name=gripper_name, silent=silent)
    elif type_of_quality == 'number_of_contacts':
        quality[quality_key] = grasp_quality_point_contacts(
            transforms, collisions, object_mesh=mesh, gripper_name=gripper_name, silent=silent)
    else:
        raise Exception("Quality metric unknown: ", quality)

    # Filter out by quality
    quality_np = np.array(quality[quality_key])
    collisions = np.array(collisions)

    f_points = []
    f_normals = []
    f_transforms = []
    f_roll_angles = []
    f_standoffs = []
    f_collisions = []
    f_quality = []

    for i, _ in enumerate(transforms):
        if quality_np[i] >= min_quality:
            f_points.append(points[i])
            f_normals.append(normals[i])
            f_transforms.append(transforms[i])
            f_roll_angles.append(roll_angles[i])
            f_standoffs.append(standoffs[i])
            f_collisions.append(int(collisions[i]))
            f_quality.append(quality_np[i])

    points = np.array(f_points)
    normals = np.array(f_normals)
    transforms = np.array(f_transforms)
    roll_angles = np.array(f_roll_angles)
    standoffs = np.array(f_standoffs)
    collisions = f_collisions
    quality[quality_key] = f_quality

    return points, normals, transforms, roll_angles, standoffs, collisions, quality


def make_parser():
    """Create program arguments and default values.

    Returns:
        argparse.ArgumentParser -- an argument parser
    """
    parser = argparse.ArgumentParser(description='Sample grasps for an object.',
                                     formatter_class=argparse.ArgumentDefaultsHelpFormatter)
    parser.add_argument('--object_file', type=str,
                        default='/home/arsalan/data/models_selected/03797390/1be6b2c84cdab826c043c2d07bb83fc8/model.obj',
                        help='Number of samples.')
    parser.add_argument('--dataset', type=str, default='UNKNOWN',
                        help='Metadata about the origin of the file.')
    parser.add_argument('--classname', type=str, default='UNKNOWN',
                        help='Metadata about the class of the object.')
    parser.add_argument('--scale', type=float, default=1.0,
                        help='Scale the object.')
    parser.add_argument('--resize', type=float,
                        help="""Resize the object, such that the longest of its \
                            bounding box dimensions is of length --resize.""")
    parser.add_argument('--use_stl', action='store_true',
                        help='Use STL instead of obj.')
    parser.add_argument('--gripper', choices=get_available_grippers().keys(), default='panda',
                        help='Type of gripper.')
    parser.add_argument('--quality', choices=['number_of_contacts', 'antipodal'],
                        default='number_of_contacts',
                        help='Which type of quality metric to evaluate.')

    parser.add_argument('--single_standoff', action='store_true',
                        help='Use the closest possible standoff.')

    parser.add_argument('--systematic_sampling', action='store_true',
                        help='Systematically sample stuff.')
    parser.add_argument('--systematic_surface_density', type=float, default=0.005*0.005,
                        help='Surface density used for systematic sampling (in square meters).')
    parser.add_argument('--systematic_standoff_density', type=float, default=0.01,
                        help='Standoff density used for systematic sampling (in meters).')
    parser.add_argument('--systematic_roll_density', type=float, default=15.,
                        help='Roll density used for systematic sampling (in degrees).')
    parser.add_argument('--filter_best_per_position', action='store_true',
                        help='Only store one grasp (highest quality) if there are multiple per with the same position.')

    parser.add_argument('--min_quality', type=float, default=0.0,
                        help="""Only store grasps whose quality is at least this value. \
                            Colliding grasps have quality -1, i.e. they are filtered out by default.""")

    parser.add_argument('--num_samples', type=int, default=10,
                        help='Number of samples.')
    parser.add_argument('--output', type=str, default="tmp.json",
                        help='File to store the results (json).')
    parser.add_argument('--add_quality_metric', nargs=2, type=str, default="",
                        help='File (json) to calculate additional quality metric for.')
    parser.add_argument('--silent', action='store_true',
                        help='No commandline output.')

    parser.add_argument('--force', action='store_true',
                        help='Do things my way.')

    return parser


if __name__ == "__main__":
    parser = make_parser()
    args = parser.parse_args()

    verboseprint = print if not args.silent else lambda *a, **k: None

    if args.add_quality_metric:
        with open(args.add_quality_metric[1], 'r') as f:
            grasps = json.load(f)
        obj = Object(grasps['object'].replace('.obj', '.stl')
                     if args.use_stl else grasps['object'])
        obj.rescale(grasps['object_scale'])

        grasp_tfs = np.array(grasps['transforms'])
        collisions = np.array(grasps['collisions'])

        key = 'quality_{}'.format(args.add_quality_metric[0])

        if key in grasps.keys() and not args.force:
            raise Exception(
                "Quality metric already part of json file! (Needs --force option) ", key)

        if key == 'quality_number_of_contacts':
            grasps[key] = grasp_quality_point_contacts(
                grasp_tfs,
                collisions,
                object_mesh=obj.mesh,
                gripper_name=grasps['gripper'],
                silent=args.silent)
        elif key == 'quality_antipodal':
            grasps[key] = grasp_quality_antipodal(
                grasp_tfs,
                collisions,
                object_mesh=obj.mesh,
                gripper_name=grasps['gripper'],
                silent=args.silent)
        else:
            raise Exception("Unknown quality metric: ", key)

        with open(args.add_quality_metric[1], 'w') as f:
            json.dump(grasps, f)

    else:
        if os.path.dirname(args.output) != '':
            try:
                os.makedirs(os.path.dirname(args.output))
            except OSError as e:
                if e.errno != errno.EEXIST:
                    raise

        obj = Object(args.object_file.replace('.obj', '.stl')
                     if args.use_stl else args.object_file)

        if args.resize:
            obj.resize(args.resize)
        else:
            obj.rescale(args.scale)

        gripper = create_gripper(args.gripper)

        points, normals, transforms, roll_angles, standoffs, collisions, qualities\
            = sample_multiple_grasps(args.num_samples,
                                     obj.mesh,
                                     gripper_name=args.gripper,
                                     systematic_sampling=args.systematic_sampling,
                                     roll_density=args.systematic_roll_density,
                                     standoff_density=args.systematic_standoff_density,
                                     surface_density=args.systematic_surface_density,
                                     type_of_quality=args.quality,
                                     filter_best_per_position=args.filter_best_per_position,
                                     min_quality=args.min_quality,
                                     silent=args.silent)

        # save transforms
        grasps = {
            'object': obj.filename,
            'object_scale': obj.scale,
            'object_class': args.classname,
            'object_dataset': args.dataset,
            'gripper': args.gripper,
            'gripper_configuration': [gripper.q],
            'transforms': [t.tolist() for t in transforms],
            'roll_angles': roll_angles.tolist(),
            'standoffs': standoffs.tolist(),
            'mesh_points': [p.tolist() for p in points],
            'mesh_normals': [n.tolist() for n in normals],
            'collisions': collisions,
        }
        grasps.update(qualities)

        with open(args.output, 'w') as f:
            verboseprint("Writing results to:", args.output)
            json.dump(grasps, f)
