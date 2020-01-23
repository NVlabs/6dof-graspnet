# Copyright (c) 2019, NVIDIA CORPORATION.  All rights reserved.
#
# NVIDIA CORPORATION and its licensors retain all intellectual property
# and proprietary rights in and to this software, related documentation
# and any modifications thereto.  Any use, reproduction, disclosure or
# distribution of this software and related documentation without an express
# license agreement from NVIDIA CORPORATION is strictly prohibited.
from __future__ import print_function

import numpy as np
import yaml
import argparse
import numpy
import grasp_estimator
import copy
import sys
import os
import grasp_data_reader
import tensorflow as tf
import glob
import sample
import json
import subprocess
import time
import datetime
import os
from sklearn.metrics import precision_recall_curve, average_precision_score
from scipy import spatial
import shutil

RADIUS = 0.02

def default(obj):
    if type(obj).__module__ == np.__name__:
        if isinstance(obj, np.ndarray):
            return obj.tolist()
        else:
            return obj.item()
    raise TypeError('Unknown type:', type(obj))


def create_directory(path, delete_if_exist=False):
    if not os.path.isdir(path):
        os.makedirs(path)
    else:
        if delete_if_exist:
            print('***************** deleting folder ', path)
            shutil.rmtree(path)
            os.makedirs(path)


def make_parser(argv):
    """
      Outputs a parser.
    """
    parser = argparse.ArgumentParser(description='Evaluators',
                                     formatter_class=argparse.ArgumentDefaultsHelpFormatter
                                    )
    parser.add_argument('--vae_training_folder', type=str, default='')
    parser.add_argument('--evaluator_training_folder', type=str, default='')
    parser.add_argument('--eval_data_folder', type=str, default='')
    parser.add_argument('--generate_data_if_missing', type=int, default=0)
    parser.add_argument('--dataset_root_folder', type=str, default='')
    parser.add_argument('--num_experiments', type=int, default=100)
    parser.add_argument('--eval_split', type=str, default='test')
    parser.add_argument('--eval_grasp_evaluator', type=int, default=0)
    parser.add_argument('--eval_vae_and_evaluator', type=int, default=1)
    parser.add_argument('--output_folder', type=str, default='')
    parser.add_argument('--gradient_based_refinement', action='store_true', default=False)

    return parser.parse_args(argv[1:])



class Evaluator():
    def __init__(self, cfg, create_data_if_not_exist, output_folder,
        eval_experiment_folder, num_experiments, eval_grasp_evaluator=False,
        eval_vae_and_evaluator=True):
        self._should_create_data = create_data_if_not_exist
        self._eval_experiment_folder = eval_experiment_folder
        self._num_experiments = num_experiments
        self._grasp_reader = grasp_data_reader.PointCloudReader(
            root_folder=cfg.dataset_root_folder,
            batch_size=cfg.num_grasps_per_object,
            num_grasp_clusters=cfg.num_grasp_clusters,
            npoints=cfg.npoints,
            min_difference_allowed=(0, 0, 0),
            max_difference_allowed=(3, 3, 0),
            occlusion_nclusters=0,
            occlusion_dropout_rate=0.,
            use_uniform_quaternions=False,
            ratio_of_grasps_used=1,
        )
        self._cfg = cfg
        self._grasp_estimator = grasp_estimator.GraspEstimator(cfg)
        os.environ['CUDA_VISIBLE_DEVICES'] = str(self._cfg.gpu)
        self._sess = tf.Session()
        del os.environ['CUDA_VISIBLE_DEVICES']
        self._grasp_estimator.build_network()
        self._eval_grasp_evaluator = eval_grasp_evaluator
        self._eval_vae_and_evaluator = eval_vae_and_evaluator
        self._flex_initialized = False
        self._output_folder = output_folder
        self.update_time_stamp()
        

    def read_eval_scene(self, file_path, visualize=False):
        if not os.path.isfile(file_path):
            if not self._should_create_data:
                raise ValueError('could not find data {}'.format(file_path))
            
            json_path = self._grasp_reader.generate_object_set(self._cfg.eval_split)
            obj_grasp_data = self._grasp_reader.read_grasp_file(
                os.path.join(self._cfg.dataset_root_folder, json_path), True)
            obj_pose = self._grasp_reader.arrange_objects(obj_grasp_data[-3])[0]
            in_camera_pose = None
            print('changing object to ', obj_grasp_data[-2])
            self._grasp_reader.change_object(obj_grasp_data[-2], obj_grasp_data[-1])
            pc, camera_pose, in_camera_pose= self._grasp_reader.render_random_scene(None)
            folder_path = file_path[:file_path.rfind('/')]
            create_directory(folder_path)

            print('writing {}'.format(file_path))
            np.save(file_path, {'json': json_path, 'obj_pose': obj_pose, 'camera_pose': in_camera_pose})
        else:
            d = np.load(file_path).item()
            json_path = d['json']
            obj_pose = d['obj_pose']
            obj_grasp_data = self._grasp_reader.read_grasp_file(
                os.path.join(self._cfg.dataset_root_folder, json_path), True
            )
            in_camera_pose = d['camera_pose']
            self._grasp_reader.change_object(obj_grasp_data[-2], obj_grasp_data[-1])
            pc, camera_pose, _= self._grasp_reader.render_random_scene(in_camera_pose)
        

        pos_grasps = np.matmul(np.expand_dims(camera_pose, 0), obj_grasp_data[0])
        neg_grasps = np.matmul(np.expand_dims(camera_pose, 0), obj_grasp_data[2])
        grasp_labels = np.hstack(
            (np.ones(pos_grasps.shape[0]), np.zeros(neg_grasps.shape[0]))).astype(np.int32)
        grasps = np.concatenate((pos_grasps, neg_grasps), 0)
        
        if visualize:
            from visualization_utils import draw_scene
            import mayavi.mlab as mlab

            pos_mask = np.logical_and(grasp_labels == 1, np.random.rand(*grasp_labels.shape) < 0.1)
            neg_mask = np.logical_and(grasp_labels == 0, np.random.rand(*grasp_labels.shape) < 0.01)

            print(grasps[pos_mask, :, :].shape, grasps[neg_mask, :, :].shape)
            draw_scene(pc, grasps[pos_mask, :, :])
            mlab.show()

            draw_scene(pc, grasps[neg_mask, :, :])
            mlab.show()

        return pc[:, :3], grasps, grasp_labels, {'cad_path': obj_grasp_data[-2], 'cad_scale': obj_grasp_data[-1], 'to_canonical_transformation': grasp_data_reader.inverse_transform(camera_pose)}
    

    def eval_scene(self, file_path, visualize=False):
        """
          Returns full_results, evaluator_results.
            full_results: Contains information about grasps in canonical pose, scores,
              ground truth positive grasps, and also cad path and scale that is used for
              flex evaluation.
            evaluator_results: Only contains information for the classification of positive
              and negative grasps. The info is gt label of each grasp, predicted score for
              each grasp, and the 4x4 transformation of each grasp.
        """
        pc, grasps, grasps_label, flex_info = self.read_eval_scene(file_path)
        canonical_transform = flex_info['to_canonical_transformation']
        evaluator_result = None
        full_results = None
        if self._eval_grasp_evaluator:
            latents = self._grasp_estimator.sample_latents()
            output_grasps, output_scores, _ = self._grasp_estimator.predict_grasps(
                self._sess,
                pc,
                latents,
                0,
                grasps_rt=grasps
            )
            evaluator_result = (grasps_label, output_scores, output_grasps)


        if self._eval_vae_and_evaluator:
            latents = np.random.rand(self._cfg.num_samples, self._cfg.latent_size) * 4 - 2
            print(pc.shape)
            generated_grasps, generated_scores, _ = self._grasp_estimator.predict_grasps(
                self._sess,
                pc,
                latents,
                num_refine_steps=self._cfg.num_refine_steps,
            )

            gt_pos_grasps = [g for g, l in zip(grasps, grasps_label) if l == 1]
            gt_pos_grasps = np.asarray(gt_pos_grasps).copy()
            gt_pos_grasps_canonical = np.matmul(canonical_transform, gt_pos_grasps)
            generated_grasps = np.asarray(generated_grasps)
            print(generated_grasps.shape)
            generated_grasps_canonical = np.matmul(canonical_transform, generated_grasps)
            

            obj = sample.Object(flex_info['cad_path'])
            obj.rescale(flex_info['cad_scale'])
            mesh = obj.mesh
            mesh_mean = np.mean(mesh.vertices, 0, keepdims=True)

            canonical_pc = pc.dot(canonical_transform[:3, :3].T)
            canonical_pc += np.expand_dims(canonical_transform[:3, 3], 0)

            gt_pos_grasps_canonical[:, :3, 3] += mesh_mean
            canonical_pc += mesh_mean
            generated_grasps_canonical[:, :3, 3] += mesh_mean
            
            if visualize:
                from visualization_utils import draw_scene
                import mayavi.mlab as mlab

                draw_scene(canonical_pc, grasps=gt_pos_grasps_canonical, mesh=mesh)
                mlab.show()

                
                draw_scene(canonical_pc + mesh_mean, grasps=generated_grasps_canonical, mesh=mesh, grasp_scores=generated_scores)
                mlab.show()
            
            full_results = (generated_grasps_canonical, generated_scores, gt_pos_grasps_canonical, flex_info['cad_path'], flex_info['cad_scale'])

        return full_results, evaluator_result

    
    def update_time_stamp(self):
        self._signature = self._get_current_time_stamp()


    def _get_current_time_stamp(self):
        """No Comments."""
        now = datetime.datetime.now()
        return now.strftime("%Y-%m-%d_%H-%M")


    def eval_all(self, plot_curves):
        """
        Evaluates all of the test scenes.

        Args:
          plot_curves: bool, if True, plots the coressponding figure
            for each of the evaluations.
        """
        self._grasp_estimator.load_weights(self._sess)
        
        create_directory(self._eval_experiment_folder)
        
        num_digits = len(str(self._num_experiments))
        
        all_eval_results = []
        all_full_results = []
        
        for i in range(self._num_experiments):
            full_result, eval_result = self.eval_scene(os.path.join(self._eval_experiment_folder, 'eval_configs', str(i).zfill(num_digits)) + '.npy', False)
            if full_result is not None:
                grasps, scores, gt_grasps, cad_path, cad_scale = full_result
                experiment_folder = os.path.join(self._eval_experiment_folder, 'flex_folder', str(i).zfill(num_digits))
                flex_outcomes = self.eval_grasps_on_flex(grasps, cad_path, cad_scale, experiment_folder)
                all_full_results.append((grasps, scores, flex_outcomes, gt_grasps))
            
            if eval_result is not None:
                all_eval_results.append(eval_result)
        
        if len(all_eval_results) > 0:
            self.metric_classification_mean_ap(
                [x[0] for x in all_eval_results],
                [x[1] for x in all_eval_results], 
                plot_curves)
        
        if len(all_full_results) > 0:
            self.metric_coverage_success_rate(
                [x[0] for x in all_full_results],
                [x[1] for x in all_full_results],
                [x[2] for x in all_full_results],
                [x[3] for x in all_full_results],
                plot_curves
            )


    def metric_classification_mean_ap(self, gt_labels_list, scores_list, visualize):
        """
        Computes the average precision metric for evaluator.

        Args:
          gt_labels_list: list of binary numbers indicating the success of 
            grasps. 1 means grasp is successful and 0 means failure.
          scores_list: list of float numbers for the score of each grasp.
          visualize: bool, if True, visualizes the plots.
        
        Returns:
          average_precision: area under the curve for precision-recall plot.
          best_threshold: float, best threshold that has the highest f-1 
            measure.
        """
        all_gt_labels = []
        all_scores = []
        if len(gt_labels_list) != len(scores_list):
            raise ValueError("Length of the lists should match")
        
        for gt_labels in gt_labels_list:
            all_gt_labels += [l for l in gt_labels]
        for scores in scores_list:
            all_scores += [s for s in scores]
        
        precision, recall, thresholds = precision_recall_curve(all_gt_labels, all_scores)
        average_precision = average_precision_score(all_gt_labels, all_scores)
        f1_score = 2 * (precision * recall) / (precision + recall)

        best_threshold = thresholds[np.argmax(f1_score)]
        if visualize:
            import matplotlib.pyplot as plt
            plt.plot(recall, precision)

            plt.xlabel('Recall')
            plt.ylabel('Precision')
            plt.ylim([0.0, 1.05])
            plt.xlim([0.0, 1.0])
            plt.title('2-class Precision-Recall curve: AP={0:02f}, best_treshold = {0:02f}'.format(
                    average_precision, best_threshold))
            plt.show()
        
        np.save(
            os.path.join(self._output_folder, '{}_evalauator.npy'.format(self._signature)),
            {'cfg':self._cfg, 'precisions': precision, 'recalls': recall, 'average_precision': average_precision, 'best_threshold': best_threshold}
        )
        
        


        return average_precision, best_threshold
    
    
    def metric_coverage_success_rate(self, grasps_list, scores_list, flex_outcomes_list, gt_grasps_list, visualize):
        """
        Computes the coverage success rate for grasps of multiple objects.

        Args:
          grasps_list: list of numpy array, each numpy array is the predicted
            grasps for each object. Each numpy array has shape (n, 4, 4) where
            n is the number of predicted grasps for each object.
          scores_list: list of numpy array, each numpy array is the predicted
            scores for each grasp of the corresponding object.
          flex_outcomes_list: list of numpy array, each element of the numpy
            array indicates whether that grasp succeeds in grasping the object
            or not.
          gt_grasps_list: list of numpy array. Each numpy array has shape of
            (m, 4, 4) where m is the number of groundtruth grasps for each
            object.
          visualize: bool. If True, it will plot the curve.
        
        Returns:
          auc: float, area under the curve for the success-coverage plot.
        """
        all_trees = []
        all_grasps = []
        all_object_indexes = []
        all_scores = []
        all_flex_outcomes = []
        visited = set()
        tot_num_gt_grasps = 0
        for i in range(len(grasps_list)):
            print('building kd-tree {}/{}'.format(i, len(grasps_list)))
            gt_grasps = np.asarray(gt_grasps_list[i]).copy()
            all_trees.append(spatial.KDTree(gt_grasps[:, :3, 3]))
            tot_num_gt_grasps += gt_grasps.shape[0]

            for g, s, f in zip(grasps_list[i], scores_list[i], flex_outcomes_list[i]):
                all_grasps.append(np.asarray(g).copy())
                all_object_indexes.append(i)
                all_scores.append(s)
                all_flex_outcomes.append(f)
        
        
        all_grasps = np.asarray(all_grasps)
        
        all_scores = np.asarray(all_scores)
        order = np.argsort(-all_scores)
        num_covered_so_far = 0
        correct_grasps_so_far = 0
        num_visited_grasps_so_far = 0

        precisions = []
        recalls = []
        prev_score = None

        for oindex, index in enumerate(order):
            if oindex % 1000 == 0:
                print(oindex, len(order))
            
            object_id = all_object_indexes[index]
            close_indexes = all_trees[object_id].query_ball_point(all_grasps[index, :3, 3], RADIUS)

            num_new_covered_gt_grasps = 0

            for close_index in close_indexes:
                key = (object_id, close_index)
                if key in visited:
                    continue
                
                visited.add(key)
                num_new_covered_gt_grasps += 1

            correct_grasps_so_far += all_flex_outcomes[index]
            num_visited_grasps_so_far += 1
            num_covered_so_far += num_new_covered_gt_grasps

            if prev_score is not None and abs(prev_score - all_scores[index]) < 1e-3:
                precisions[-1] = float(correct_grasps_so_far) / num_visited_grasps_so_far
                recalls[-1] = float(num_covered_so_far) / tot_num_gt_grasps
            else:
                precisions.append(float(correct_grasps_so_far) / num_visited_grasps_so_far)
                recalls.append(float(num_covered_so_far) / tot_num_gt_grasps)
                prev_score = all_scores[index]
        
        auc = 0
        for i in range(1, len(precisions)):
            auc += (recalls[i] - recalls[i-1]) * (precisions[i] + precisions[i-1]) * 0.5
        
        if visualize:
            import matplotlib.pyplot as plt
            plt.plot(recalls, precisions)
            plt.title('auc = {0:02f}'.format(auc))
            plt.ylim([0.0, 1.05])
            plt.xlim([0.0, 1.0])
            plt.show()
        
        print('auc = {}'.format(auc))
        np.save(
            os.path.join(self._output_folder, '{}_vae+evaluator.npy'.format(self._signature)),
            {'precisions': precisions, 'recalls': recalls, 'auc': auc, 'cfg': self._cfg}
        )
        
        return auc

    
    def eval_grasps_on_flex(self, grasps, cad_path, cad_scale, experiment_folder):
        """
        Evaluates the graps on flex physics engine and determines whether the
        grasps will succeed or not.

        Args:
          grasps: numpy array list of grasps for an object.
          cad_path: string, path to the obj/stl file of the object.
          cad_scale: float, the scale that is applied to the mesh of the 
            object.
          experiment_folder: the folder that is used to copy the temp files
            necessary for running the jobs and also aggregating the results.
        
        Returns:
          grasp_success: list of binary numbers. 0 means that the grasp failed,
            and 1 means that the grasp succeeded.
        """
        raise NotImplementedError("The code for grasp evaluation is not released")


    def __del__(self):
        del self._grasp_reader


if __name__ == '__main__':
    args = make_parser(sys.argv)
    create_directory(args.output_folder)
    
    
    cfg = grasp_estimator.joint_config(
        args.vae_training_folder, args.evaluator_training_folder, 
        args.dataset_root_folder, args.eval_split)
    cfg['threshold'] = -1
    if args.gradient_based_refinement:
        cfg['num_refine_steps'] = 10
        cfg['sample_based_improvement'] = 0
    else:
        cfg['num_refine_steps'] = 20
        cfg['sample_based_improvement'] = 1
    
    evaluator = Evaluator(
        cfg,
        args.generate_data_if_missing,
        args.output_folder,
        args.eval_data_folder,
        args.num_experiments,
        eval_grasp_evaluator=args.eval_grasp_evaluator,
        eval_vae_and_evaluator=args.eval_vae_and_evaluator,
    )
    evaluator.eval_all(True)
    del evaluator
    
