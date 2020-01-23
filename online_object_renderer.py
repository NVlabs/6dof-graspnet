# Copyright (c) 2019, NVIDIA CORPORATION.  All rights reserved.
#
# NVIDIA CORPORATION and its licensors retain all intellectual property
# and proprietary rights in and to this software, related documentation
# and any modifications thereto.  Any use, reproduction, disclosure or
# distribution of this software and related documentation without an express
# license agreement from NVIDIA CORPORATION is strictly prohibited.
from __future__ import print_function
from __future__ import absolute_import

import numpy as np
import copy
import cv2
import h5py
import sample
import math
import sys
import argparse
import os
# Uncomment following line for headless rendering
# os.environ["PYOPENGL_PLATFORM"] = "egl" 
import pyrender 

import trimesh
import trimesh.transformations as tra
import multiprocessing as mp


# Cover python 2.7 and python 3.5
try:
    import Queue
except:
    import queue as Queue



class OnlineObjectRendererMultiProcess(mp.Process):
    def __init__(self, caching=True):
        mp.Process.__init__(self)
        self._renderer = None
        self._caching = caching
        self._queue = mp.Queue()
        self._output_queue = mp.Queue()
        self._should_stop = False
    
    
    def run(self):
        self._renderer = OnlineObjectRenderer(caching=self._caching)
        while not self._should_stop:
            try:
                request = self._queue.get(timeout=1)
            except Queue.Empty:
                continue
            if request[0] == 'render':
                self._process_render_request(request)
            elif request[0] == 'change_object':
                self._process_change_object_request(request)
            else:
                raise ValueError('unknown requst', request)
            
            

    def _process_render_request(self, render_request):
        pose = render_request[1]
        try:
            output = self._renderer.render(pose)
            self._output_queue.put(('ok', output))
        except Exception as e:
            self._output_queue.put(('no', str(e)))
    
    def _process_change_object_request(self, change_object_request):
        #print('change object', change_object_request)
        cad_path = change_object_request[1]
        cad_scale = change_object_request[2]
        try:
            self._renderer.change_object(cad_path, cad_scale)
            self._output_queue.put(('ok',))
        except Exception as e:
            self._output_queue.put(('no', str(e)))


    def render(self, pose):
        self._queue.put(('render', pose))
        
        outcome = self._output_queue.get(timeout=10)

        if outcome[0] != 'ok':
            print('------------->', outcome)
            raise ValueError(outcome[1])
        else:
            return outcome[1]
        
        
    
    def change_object(self, cad_path, cad_scale):
        self._queue.put(('change_object', cad_path, cad_scale))
        
        outcome = self._output_queue.get(timeout=10)
        if outcome[0] != 'ok':
            raise ValueError(outcome[1])
        



class OnlineObjectRenderer:
    def __init__(self, fov=np.pi / 6, caching=True):
        """
        Args:
          fov: float, 
        """
        self._fov = fov
        self.mesh = None
        self._scene = None
        self.tmesh = None
        self._init_scene()
        self._current_context = None
        self._cache = {} if caching else None
        self._caching = caching

    def _init_scene(self):
        self._scene = pyrender.Scene()
        camera = pyrender.PerspectiveCamera(yfov=self._fov, aspectRatio=1.0, znear=0.001) # do not change aspect ratio
        camera_pose = tra.euler_matrix(np.pi, 0, 0)

        self._scene.add(camera, pose=camera_pose, name='camera')

        #light = pyrender.SpotLight(color=np.ones(4), intensity=3., innerConeAngle=np.pi/16, outerConeAngle=np.pi/6.0)
        #self._scene.add(light, pose=camera_pose, name='light')

        self.renderer = pyrender.OffscreenRenderer(400, 400)


    def _load_object(self, path, scale):
        if (path, scale) in self._cache:
            return self._cache[(path, scale)]
        obj = sample.Object(path)
        obj.rescale(scale)
        
        tmesh = obj.mesh
        tmesh_mean = np.mean(tmesh.vertices, 0)
        tmesh.vertices -= np.expand_dims(tmesh_mean, 0)

        lbs = np.min(tmesh.vertices, 0)
        ubs = np.max(tmesh.vertices, 0)
        object_distance = np.max(ubs - lbs) * 5

        mesh = pyrender.Mesh.from_trimesh(tmesh)

        context = {
            'tmesh': copy.deepcopy(tmesh), 
            'distance': object_distance, 
            'node': pyrender.Node(mesh=mesh),
            'mesh_mean': np.expand_dims(tmesh_mean, 0),
        }
        
        self._cache[(path, scale)] = context
        return self._cache[(path, scale)]
    

    def change_object(self, path, scale):
        if self._current_context is not None:
            self._scene.remove_node(self._current_context['node'])

        if not self._caching:
            self._cache = {}
        
        self._current_context = self._load_object(path, scale)
        self._scene.add_node(self._current_context['node'])
    
    
    def current_context(self):
        return self._current_context

    
    def _to_pointcloud(self, depth):
        fy = fx = 0.5 / np.tan(self._fov * 0.5) # aspectRatio is one.
        height = depth.shape[0]
        width = depth.shape[1]

        mask = np.where(depth > 0)
        
        x = mask[1]
        y = mask[0]
        
        normalized_x = (x.astype(np.float32) - width * 0.5) / width
        normalized_y = (y.astype(np.float32) - height * 0.5) / height
        
        world_x = normalized_x * depth[y, x] / fx
        world_y = normalized_y * depth[y, x] / fy
        world_z = depth[y, x]
        ones = np.ones(world_z.shape[0], dtype=np.float32)

        return np.vstack((world_x, world_y, world_z, ones)).T
        

    def render(self, pose, render_pc=True):
        if self._current_context is None:
            raise ValueError('invoke change_object first')

        transferred_pose = pose.copy()
        transferred_pose[2, 3] = self._current_context['distance']
        self._scene.set_pose(self._current_context['node'], transferred_pose)
        
        color, depth = self.renderer.render(self._scene)
        
        if render_pc:
            pc = self._to_pointcloud(depth)
        else:
            pc = None

        return color, depth, pc, transferred_pose

    def render_canonical_pc(self, poses):
        all_pcs = []
        for pose in poses:
            _, _, pc, pose = self.render(pose)
            pc = pc.dot(inverse_transform(pose).T)
            all_pcs.append(pc)
        all_pcs = np.concatenate(all_pcs, 0)
        return all_pcs
    
    def start(self):
        pass
    
    def join(self):
        pass
    
    def terminate(self):
        pass
        
    
    

def inverse_transform(trans):
    rot = trans[:3, :3]
    t = trans[:3, 3]
    rot = np.transpose(rot)
    t = -np.matmul(rot, t)
    output = np.zeros((4, 4), dtype=np.float32)
    output[3][3] = 1
    output[:3, :3] = rot
    output[:3, 3] = t
    return output

def main_check_pointcloud(iterate_all_viewpoints=True):
    from visualization_utils import draw_scene
    import mayavi.mlab as mlab 
    import matplotlib.pyplot as plt
    import glob
    import random
    import cv2
    import time
    import json

    quaternions = [l[:-1].split('\t') for l in open('uniform_quaternions/data2_4608.qua', 'r').readlines()]

    quaternions = [[float(t[0]),
                    float(t[1]),
                    float(t[2]),
                    float(t[3])] for t in quaternions]
    quaternions = np.asarray(quaternions)
    quaternions = np.roll(quaternions, 1, axis=1)
    all_eulers = [tra.quaternion_matrix(q) for q in quaternions]
    all_eulers = []
    for az in np.linspace(0, math.pi * 2, 30):
        for el in np.linspace(-math.pi / 2, math.pi / 2, 30):
            all_eulers.append(tra.euler_matrix(el, az, 0))

    renderer = OnlineObjectRendererMultiProcess(caching=True)
    renderer.start()
    
    
    grasps_path = glob.glob('unified_grasp_data/grasps/*.json')
    random.shuffle(grasps_path)
    #mesh_paths = ['unified_grasp_data/meshes/Bowl/9a52843cc89cd208362be90aaa182ec6.stl']
    

    for main_iter in range(5*len(grasps_path)):
        gpath = grasps_path[main_iter%len(grasps_path)]
        json_dict = json.load(open(gpath))
        mpath = os.path.join('unified_grasp_data', json_dict['object'])
        scale = json_dict['object_scale']
        #mpath = 'unified_grasp_data/meshes/Bottle/10d3d5961e00b2133ff038bc77759685.stl'
        #mpath = 'unified_grasp_data/meshes/Bottle/ef631a2ce94fae3ab8966911a5afa22c.stl'
        print(main_iter, mpath)
        start_time = time.time()
        renderer.change_object(mpath, scale)
        if iterate_all_viewpoints == True:
            viewpoints = all_eulers
        else:
            viewpoints = [all_eulers[np.random.randint(len(all_eulers))]]
        
        for view in viewpoints:
            image, depth, pc, _ = renderer.render(view)
            print(time.time() - start_time)
            print('depth min = {} max = {} npoints = {}'.format(np.min(depth), np.max(depth), pc.shape))
            draw_scene(pc, None)
            mlab.show()
            
            
    
    renderer.terminate()
    renderer.join()
        

def main_check_offscreen():
    renderer = OnlineObjectRenderer(caching=False)
    #renderer = OnlineObjectRendererMultiProcess(caching=False)
    renderer.start()
    renderer.change_object('panda_gripper.obj', 1)
    output = renderer.render(np.eye(4))
    cv2.imwrite('docker.png', output[0])
    renderer.terminate()
    renderer.join()


if __name__ == '__main__':
    main_check_offscreen()

    
    



