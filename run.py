__author__ = 'slhuang'
import os
import subprocess

def profiling(dir_path, endwith):
    files=sorted(os.listdir(dir_path))
    print (files)
    artifacts = ''
    for f in files:
        if f.endswith(endwith): #f.startswith('repo'): #
            artifacts += dir_path + f + ','
    print ("----------profiling ----------")
    proc = subprocess.Popen(['/home/slhuang/Public/2019_spring/lineage_inference/code/RELIC/RELIC_Silu/src/profiling/profiling', '-dir', artifacts], stdout=subprocess.PIPE)
    output = proc.communicate()[0]
    print('Got stdout:', str(output).replace('\\r', '\n').replace('\\n', '\n'))


def pre_clustering(clustering_strategy):
    print ("---------pre_clustering----------")
    proc = subprocess.Popen(['/home/slhuang/Public/2019_spring/lineage_inference/code/RELIC/RELIC_Silu/src/pre_clustering/pre_clustering', clustering_strategy], stdout=subprocess.PIPE)
    output = proc.communicate()[0]
    print('Got stdout:', str(output).replace('\\r', '\n').replace('\\n', '\n'))

def clear_files():
    cmd = 'rm /home/slhuang/Public/2019_spring/lineage_inference/code/RELIC/RELIC_Silu/src/profiling/*.csv /home/slhuang/Public/2019_spring/lineage_inference/code/RELIC/RELIC_Silu/src/pre_clustering/*.csv /home/slhuang/Public/2019_spring/lineage_inference/code/RELIC/RELIC_Silu/src/preserving_ops/result/*.csv /home/slhuang/Public/2019_spring/lineage_inference/code/RELIC/RELIC_Silu/src/preserving_ops/*.csv'
    os.system(cmd)

def preserving_ops(dir_path, endwith):
    files=sorted(os.listdir(dir_path))
    print (files)
    artifacts = ''
    for f in files:
        if f.endswith(endwith): #f.startswith('repo'): #
            artifacts += dir_path + f + ','
    print ("----------preserving-ops----------")
    proc = subprocess.Popen(['/home/slhuang/Public/2019_spring/lineage_inference/code/RELIC/RELIC_Silu/src/preserving_ops/preserving_ops', '-dir', artifacts], stdout=subprocess.PIPE)
    output = proc.communicate()[0]
    print('Got stdout:', str(output).replace('\\r', '\n').replace('\\n', '\n'))

def run():
    #convert_windows_to_linux('/home/slhuang/Public/2018-fall/lineage_inference/case_study/retail/')
    ### retail
    #profiling('/home/slhuang/Public/2018-fall/lineage_inference/case_study/retail/', '_linux.csv') #nb_123977/artifacts/')
    #pre_clustering('-exact_schema')
    #preserving_ops('/home/slhuang/Public/2018-fall/lineage_inference/case_study/retail/') #nb_123977/artifacts/') # /repo_user/')
    
    ### nb_123977
    #profiling('/home/slhuang/Public/2018-fall/lineage_inference/case_study/nb_123977/artifacts/', '.csv') #retail/') #nb_123977/artifacts/')
    #pre_clustering('-exact_schema')
    #preserving_ops('/home/slhuang/Public/2018-fall/lineage_inference/case_study/nb_123977/artifacts/', '.csv') #nb_123977/artifacts/') # /repo_user/')

    clear_files()
    profiling('/home/slhuang/Public/2019_spring/lineage_inference/code/RELIC/RELIC_Silu/dataset/', '.csv') #retail/') #nb_123977/artifacts/')
    pre_clustering('-exact_schema')  #('-no_pre_cluster')#
    preserving_ops('/home/slhuang/Public/2019_spring/lineage_inference/code/RELIC/RELIC_Silu/dataset/', '.csv') #nb_123977/artifacts/') # /repo_user/')
    
if __name__ == '__main__':
    run()