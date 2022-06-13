pipeline {
    agent none
    
    options {
        timestamps() 
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
                CMAKE='C:\\Program Files\\CMake\\bin\\cmake.exe'
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
                    
                    cd OTRGui
                    mkdir build
                    cd build
                    
                    "${env.CMAKE}" ..
                    "${env.CMAKE}" --build . --config Release
                    
                    cd "..\\..\\"
                    
                    move "soh\\Release\\soh.exe" ".\\"
                    move "OTRGui\\build\\assets" ".\\"
                    move ".\\OTRExporter\\x64\\Release\\ZAPD.exe" ".\\assets\\extractor\\"
                    move ".\\OTRGui\\build\\Release\\OTRGui.exe" ".\\"
                    rename README.md readme.txt
                    
                    "${env.ZIP}" a soh.7z soh.exe OTRGui.exe assets readme.txt
                    
                    """
                    archiveArtifacts artifacts: 'soh.7z', followSymlinks: false, onlyIfSuccessful: true
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
                    
                    cp ../../ZELOOTD.z64 OTRExporter/baserom_non_mq.z64
                    docker build . -t soh
                    docker run --name sohcont -dit --rm -v $(pwd):/soh soh /bin/bash
                    cp ../../buildsoh.bash soh
                    docker exec sohcont soh/buildsoh.bash
                    
                    mkdir build
                    mv soh/soh.elf build/
                    mv OTRGui/build/OTRGui build/
                    mv OTRGui/build/assets build/
                    mv ZAPDTR/ZAPD.out build/assets/extractor/
                    mv README.md build/readme.txt
                    cd build

                    7z a soh-linux.7z soh.elf OTRGui assets readme.txt
                    mv soh-linux.7z ../
                    
                    '''
                }
                sh 'sudo docker container stop sohcont'
                archiveArtifacts artifacts: 'soh-linux.7z', followSymlinks: false, onlyIfSuccessful: true
            }
            post {
                always {
                    step([$class: 'WsCleanup']) // Clean workspace
                }
            }
        }
    }
}
