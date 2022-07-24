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
                        PLATFORM='x64'
                        PYTHON='C:\\Users\\jenkins\\AppData\\Local\\Programs\\Python\\Python310\\python.exe'
                        CMAKE='C:\\Program Files\\CMake\\bin\\cmake.exe'
                        CPACK='C:\\Program Files\\CMake\\bin\\cpack.exe'
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
                            
                            "${env.CMAKE}" -S . -B "build\\${env.PLATFORM}" -G "Visual Studio 17 2022" -T ${env.TOOLSET} -A ${env.PLATFORM} -D PYTHON_LIBRARIES=${env.PYTHON}
                            "${env.CMAKE}" --build ".\\build\\${env.PLATFORM}" --config Release
                            cd  ".\\build\\${env.PLATFORM}"
                            "${env.CPACK}" -G ZIP
                            cd "..\\..\\"

                            move "_packages\\*.zip" "soh.zip"
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
                            
                            mv README.md readme.txt		
                            mv _packages/*.appimage SoH.AppImage

                            7z a soh-linux.7z SoH.AppImage readme.txt
                            
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
<<<<<<< HEAD
                            cd soh
                            make setup -j$(sysctl -n hw.physicalcpu) OPTFLAGS=-O2 DEBUG=0 LD="ld"
                            make -j$(sysctl -n hw.physicalcpu) DEBUG=0 OPTFLAGS=-O2 LD="ld"
                            make appbundle
                            mv ../README.md readme.txt
                            7z a soh-mac.7z soh.app readme.txt
=======

                            cmake -H. -Bbuild-cmake -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_C_COMPILER=clang -DCMAKE_OBJCXX_COMPILER=clang++ -DCMAKE_OSX_ARCHITECTURES="arm64;x86_64"
                            (cd build-cmake && cpack)

                            mv README.md readme.txt		
                            mv _packages/*.dmg SoH.dmg
                            
                            7z a soh-mac.7z SoH.dmg readme.txt
>>>>>>> aaa1da15 (Add distribution of LInux & macOS)
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
                            docker build . -t sohswitch
                            docker run --name sohcont -dit --rm -v $(pwd):/soh sohswitch /bin/bash
                            docker exec sohcont scripts/switch/build.sh
                            
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

