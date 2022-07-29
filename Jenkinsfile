pipeline {
    agent none
    
    options {
        timestamps() 
        skipDefaultCheckout(true)
        disableConcurrentBuilds(abortPrevious: true)
    }
    
    stages {
        stage('Build SoH') {
            parallel {
                stage ('Build Windows') {
                    options {
                        timeout(time: 20)
                    }
                    environment {
                        MSBUILD='C:\\Program Files\\Microsoft Visual Studio\\2022\\Community\\Msbuild\\Current\\Bin\\msbuild.exe'
                        CONFIG='Release'
                        OTRPLATFORM='x64'
                        PLATFORM='x64'
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
                            
                            move "soh\\x64\\Release\\soh.exe" ".\\"
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
                    options {
                        timeout(time: 20)
                    }
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
                            docker exec sohcont scripts/linux/build.sh
                            
                            mkdir build
                            mv soh/soh.elf build/
                            mv OTRGui/build/OTRGui build/
                            mv OTRGui/build/assets build/
                            mv ZAPDTR/ZAPD.out build/assets/extractor/
                            mv README.md readme.txt
			    
                            docker exec sohcont scripts/linux/build-appimage.sh
			    
                            7z a soh-linux.7z SOH-Linux.AppImage readme.txt
                            
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
                stage ('Build macOS') {
                    agent {
                        label "SoH-Mac-Builders"
                    }
                    environment {
                        CC = 'clang -arch arm64 -arch x86_64'
                        CXX = 'clang++ -arch arm64 -arch x86_64'
                        MACOSX_DEPLOYMENT_TARGET = 10.15
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
                            cd soh
                            make setup -j$(sysctl -n hw.physicalcpu) OPTFLAGS=-O2 DEBUG=0 LD="ld"
                            make -j$(sysctl -n hw.physicalcpu) DEBUG=0 OPTFLAGS=-O2 LD="ld"
                            make appbundle
                            mv ../README.md readme.txt
                            7z a soh-mac.7z soh.app readme.txt
                            '''
                        }
                        archiveArtifacts artifacts: 'soh/soh-mac.7z', followSymlinks: false, onlyIfSuccessful: true
                    }
                    post {
                        always {
                            step([$class: 'WsCleanup']) // Clean workspace
                        }
                    }
                }
                stage ('Build Switch') {
                    options {
                        timeout(time: 20)
                    }
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
                            docker build . -t sohswitch -f Dockerfile.switch
                            docker run --name sohcont -dit --rm -v $(pwd):/soh sohswitch /bin/bash
                            docker exec sohcont .ci/switch/buildswitch.bash
                            
                            mv soh/soh.nro .
                            mv README.md readme.txt
                            
                            7z a soh-switch.7z soh.nro readme.txt
                            
                            '''
                        }
                        sh 'sudo docker container stop sohcont'
                        archiveArtifacts artifacts: 'soh-switch.7z', followSymlinks: false, onlyIfSuccessful: true
                    }
                    post {
                        always {
                            step([$class: 'WsCleanup']) // Clean workspace
                        }
                    }
                }
            }
        }
    }
}

