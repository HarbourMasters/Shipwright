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

                            xcopy "..\\..\\ZELOOTD.z64" "OTRExporter\\"
                            
                            "${env.CMAKE}" -S . -B "build\\${env.PLATFORM}" -G "Visual Studio 17 2022" -T ${env.TOOLSET} -A ${env.PLATFORM} -D Python_EXECUTABLE=${env.PYTHON}
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
                            docker exec sohcont scripts/linux/appimage/build.sh
                            
                            mv README.md readme.txt
                            mv build-cmake/*.appimage soh.appimage

                            7z a soh-linux.7z soh.appimage readme.txt
                            
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

                            cmake -H. -Bbuild-cmake -GNinja
                            cmake --build build-cmake
                            (cd build-cmake && cpack)

                            mv README.md readme.txt		
                            mv _packages/*.dmg SoH.dmg
                            
                            7z a soh-mac.7z SoH.dmg readme.txt
                            '''
                        }
                        archiveArtifacts artifacts: 'soh-mac.7z', followSymlinks: false, onlyIfSuccessful: true
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
                            
                            mv build-switch/soh/*.nro soh.nro
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

