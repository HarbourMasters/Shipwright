pipeline {
    agent none
    
    options {
        timestamps() 
        timeout(time: 30, unit: 'MINUTES')
        skipDefaultCheckout(true)
    }
    
    stages {
        stage ('Build Windows') {
            environment {
                MSBUILD='C:\\Program Files\\Microsoft Visual Studio\\2022\\Community\\Msbuild\\Current\\Bin\\msbuild.exe'
                CONFIG='Release'
                OTRPLATFORM='x64'
                PLATFORM='x86'
                ZIP='C:\\Program Files\\7-Zip\\7z.exe'
                PYTHON='C:\\Users\\jenkins\\AppData\\Local\\Programs\\Python\\Python310\\python.exe'
                TOOLSET='v142'
            }
            agent {
                label "SoH-Builders"
            }
            steps {
                checkout([
                    $class: 'GitSCM',
                    branches: scm.branches,
                    doGenerateSubmoduleConfigurations: scm.doGenerateSubmoduleConfigurations,
                    extensions: scm.extensions,
                    userRemoteConfigs: scm.userRemoteConfigs
                ])
                    
                catchError(buildResult: 'FAILURE', stageResult: 'FAILURE') {
                    bat """ 
                    
                    "${env.MSBUILD}" ".\\OTRExporter\\OTRExporter.sln" -t:build -p:Configuration=${env.CONFIG};Platform=${env.OTRPLATFORM};PlatformToolset=${env.TOOLSET};RestorePackagesConfig=true /restore /nodeReuse:false /m
                    
                    xcopy "..\\..\\ZELOOTD.z64" "OTRExporter\\"
                    
                    cd "OTRExporter"
                    "${env.PYTHON}" ".\\extract_assets.py"
                    cd "..\\"
                    
                    "${env.MSBUILD}" ".\\soh\\soh.sln" -t:build -p:Configuration=${env.CONFIG};Platform=${env.PLATFORM};PlatformToolset=${env.TOOLSET} /nodeReuse:false /m
                    
                    "${env.ZIP}" a "soh.zip" ".\\soh\\Release\\soh.exe"
                    
                    """
                    archiveArtifacts artifacts: 'soh.zip', followSymlinks: false, onlyIfSuccessful: true
                }
            }
            post {
                always {
                    step([$class: 'WsCleanup']) // Clean workspace
                }
            }
        }
        stage ('Build Linux') {
            agent {
                label "SoH-Linux-Builders"
            }
            steps {
                checkout([
                    $class: 'GitSCM',
                    branches: scm.branches,
                    doGenerateSubmoduleConfigurations: scm.doGenerateSubmoduleConfigurations,
                    extensions: scm.extensions,
                    userRemoteConfigs: scm.userRemoteConfigs
                ])
                catchError(buildResult: 'FAILURE', stageResult: 'FAILURE') {
                    sh '''
                    
                    sudo cp ../../ZELOOTD.z64 OTRExporter/baserom_non_mq.z64
                    sudo docker build . -t soh
                    sudo docker run --name sohcont -dit --rm -v $(pwd):/soh soh /bin/bash
                    sudo cp ../../buildsoh.bash soh
                    sudo docker exec sohcont soh/buildsoh.bash
                    sudo zip soh-linux.zip ./soh/soh.elf
                    
                    '''
                }
                sh 'sudo docker container stop sohcont'
                archiveArtifacts artifacts: 'soh-linux.zip', followSymlinks: false, onlyIfSuccessful: true
            }
            post {
                always {
                    step([$class: 'WsCleanup']) // Clean workspace
                }
            }
        }
    }
}
