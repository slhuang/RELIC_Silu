__author__ = 'slhuang'
import os
import subprocess

def profiling(dir_path, endwith, result_dir):
    files=sorted(os.listdir(dir_path))
    print (files)
    artifacts = ''
    for f in files:
        if f.endswith(endwith): #f.startswith('repo'): #
            artifacts += dir_path + f + ','
    print ("----------profiling ----------")
    proc = subprocess.Popen(['./src/profiling/profiling', '-dir', artifacts, '-result', result_dir], stdout=subprocess.PIPE)
    output = proc.communicate()[0]
    print('Got stdout:', str(output).replace('\\r', '\n').replace('\\n', '\n'))


def pre_clustering(clustering_strategy, result_dir):
    print ("---------pre_clustering----------")
    proc = subprocess.Popen(['./src/pre_clustering/pre_clustering', clustering_strategy, '-result', result_dir], stdout=subprocess.PIPE)
    output = proc.communicate()[0]
    print('Got stdout:', str(output).replace('\\r', '\n').replace('\\n', '\n'))

def clear_files(dir='.result/'):
    cmd = 'rm ' + dir + '*.csv'
    os.system(cmd)

def preserving_ops(dir_path, endwith, result_dir, with_pk, approx=0):
    files=sorted(os.listdir(dir_path))
    print (files)
    artifacts = ''
    for f in files:
        if f.endswith(endwith): #f.startswith('repo'): #
            artifacts += dir_path + f + ','
    print ("----------preserving-ops----------")
    if approx != 0:
        proc = subprocess.Popen(['./src/preserving_ops/preserving_ops', '-dir', artifacts, '-result', result_dir, '-approx', str(approx)], stdout=subprocess.PIPE)
    else:
        if with_pk:
            proc = subprocess.Popen(['./src/preserving_ops/preserving_ops', '-dir', artifacts, '-result', result_dir], stdout=subprocess.PIPE)
        else:
            proc = subprocess.Popen(['./src/preserving_ops/preserving_ops', '-dir', artifacts, '-result', result_dir, '-noPK'], stdout=subprocess.PIPE)
    output = proc.communicate()[0]
    print('Got stdout:', str(output).replace('\\r', '\n').replace('\\n', '\n'))

def run():
    #convert_windows_to_linux('/home/slhuang/Public/2018-fall/lineage_inference/case_study/retail/')
    ### retail
    #profiling('nb_123977/artifacts/', '.csv') #nb_123977/artifacts/')
    #profiling('/home/slhuang/Public/2018-fall/lineage_inference/case_study/retail/', '_linux.csv') #nb_123977/artifacts/')
    #pre_clustering('-exact_schema')
    #preserving_ops('/home/slhuang/Public/2018-fall/lineage_inference/case_study/retail/') #nb_123977/artifacts/') # /repo_user/')
    
    ### nb_123977
    #profiling('/home/slhuang/Public/2018-fall/lineage_inference/case_study/nb_123977/artifacts/', '.csv') #retail/') #nb_123977/artifacts/')
    #pre_clustering('-exact_schema')
    #preserving_ops('/home/slhuang/Public/2018-fall/lineage_inference/case_study/nb_123977/artifacts/', '.csv') #nb_123977/artifacts/') # /repo_user/')
    data_dir = '/Users/silu/Documents/2019-spring/lineage-inference/EXP/Real_Workflow/nb_924102.ipynb/artifacts_1/' # 924102 './dataset/'
    result_dir = "./result"
    with_pk = False
    clear_files(result_dir)
    profiling(data_dir, '.csv', result_dir) #retail/') #nb_123977/artifacts/')
    pre_clustering('-no_pre_cluster', result_dir)  #('-no_pre_cluster')# '-exact_schema'
    preserving_ops(data_dir, '.csv', result_dir, True)
    #preserving_ops('./dataset/', '.csv', result_dir, with_pk) #nb_123977/artifacts/') # /repo_user/')
    #preserving_ops('./dataset/', '.csv', result_dir, True)
    #preserving_ops('./dataset/', '.csv', result_dir, True, 100)
if __name__ == '__main__':
    run()