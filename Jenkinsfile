pipeline {
    agent none
    
    options {
        timestamps() 
        skipDefaultCheckout(true)
        disableConcurrentBuilds(abortPrevious: true)
    }
    
    stages {
        stage('Generate Assets') {
            agent {
                label "SoH-Asset-Builders"
            }
            steps {
                timeout(time: 10) {
                    checkout([
                        $class: 'GitSCM',
                        branches: scm.branches,
                        doGenerateSubmoduleConfigurations: scm.doGenerateSubmoduleConfigurations,
                        extensions: scm.extensions,
                        userRemoteConfigs: scm.userRemoteConfigs
                    ])
                    sh '''
                        cp ../../ZELOOTD.z64 OTRExporter/baserom_non_mq.z64
                        cmake --no-warn-unused-cli -H. -Bbuild-cmake -GNinja -DCMAKE_BUILD_TYPE:STRING=Release -DCMAKE_OSX_ARCHITECTURES="x86_64;arm64"
                        cmake --build build-cmake --target ExtractAssets --config Release
                    '''
                    stash includes: 'soh/assets/**/*', name: 'assets'
                }
            }
            post {
                unsuccessful {
                    step([$class: 'WsCleanup']) // Clean workspace
                }
            }
        }
        stage('Build SoH') {
            parallel {
                stage ('Build Windows') {
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
                            unstash 'assets'
                            bat """                             
                            "${env.CMAKE}" -S . -B "build\\${env.PLATFORM}" -G "Visual Studio 17 2022" -T ${env.TOOLSET} -A ${env.PLATFORM} -D Python_EXECUTABLE=${env.PYTHON} -DCMAKE_BUILD_TYPE:STRING=Release
                            "${env.CMAKE}" --build ".\\build\\${env.PLATFORM}" --target OTRGui --config Release
                            "${env.CMAKE}" --build ".\\build\\${env.PLATFORM}" --config Release
                            cd  ".\\build\\${env.PLATFORM}"
                            "${env.CPACK}" -G ZIP
                            cd "..\\..\\"

                            move "_packages\\*.zip" "soh.zip"
                            """
                        }
                        archiveArtifacts artifacts: 'soh.zip', followSymlinks: false, onlyIfSuccessful: true
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
                            unstash 'assets'
                            sh '''
                            if docker ps -aq --filter "name=sohcont" | grep -q .; then docker rm -f sohcont; fi
                            docker build . -t soh
                            docker run --name sohcont -dit --rm -v $(pwd):/soh soh /bin/bash
                            docker exec sohcont scripts/linux/appimage/build.sh
                            
                            mv README.md readme.txt
                            mv build-cmake/*.appimage soh.appimage

                            7z a soh-linux.7z soh.appimage readme.txt
                            
                            '''
                        }
                        archiveArtifacts artifacts: 'soh-linux.7z', followSymlinks: false, onlyIfSuccessful: true
                    }
                    post {
                        always {
                            sh 'sudo docker container stop sohcont'
                            sh 'docker images --quiet --filter=dangling=true | xargs --no-run-if-empty docker rmi' // Clean dangling docker images
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
                            unstash 'assets'
                            sh '''
                            cmake --no-warn-unused-cli -H. -Bbuild-cmake -GNinja -DCMAKE_BUILD_TYPE:STRING=Release -DCMAKE_OSX_ARCHITECTURES="x86_64;arm64"
                            cmake --build build-cmake --config Release --
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
                            unstash 'assets'
                            sh '''                            
                            if docker ps -aq --filter "name=sohswitchcont" | grep -q .; then docker rm -f sohswitchcont; fi
                            docker build . -t sohswitch
                            docker run --name sohswitchcont -dit --rm -v $(pwd):/soh sohswitch /bin/bash
                            docker exec sohswitchcont scripts/switch/build.sh
                            
                            mv build-switch/soh/*.nro soh.nro
                            mv README.md readme.txt
                            
                            7z a soh-switch.7z soh.nro readme.txt
                            
                            '''
                        }
                        archiveArtifacts artifacts: 'soh-switch.7z', followSymlinks: false, onlyIfSuccessful: true
                    }
                    post {
                        always {
                            sh 'sudo docker container stop sohswitchcont'
                            sh 'docker images --quiet --filter=dangling=true | xargs --no-run-if-empty docker rmi' // Clean dangling docker images
                            step([$class: 'WsCleanup']) // Clean workspace
                        }
                    }
                }
                stage ('Build Wii U') {
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
                            unstash 'assets'
                            sh '''
                            if docker ps -aq --filter "name=sohwiiucont" | grep -q .; then docker rm -f sohwiiucont; fi
                            docker build . -t sohwiiu
                            docker run --name sohwiiucont -dit --rm -v $(pwd):/soh sohwiiu /bin/bash
                            docker exec sohwiiucont scripts/wiiu/build.sh
                            
                            mv build-wiiu/soh/*.rpx soh.rpx
                            mv build-wiiu/soh/*.wuhb soh.wuhb
                            mv README.md readme.txt
                            
                            7z a soh-wiiu.7z soh.rpx soh.wuhb readme.txt
                            
                            '''
                        }
                        archiveArtifacts artifacts: 'soh-wiiu.7z', followSymlinks: false, onlyIfSuccessful: true
                    }
                    post {
                        always {
                            sh 'sudo docker container stop sohwiiucont'
                            sh 'docker images --quiet --filter=dangling=true | xargs --no-run-if-empty docker rmi' // Clean dangling docker images
                            step([$class: 'WsCleanup']) // Clean workspace
                        }
                    }
                }
            }
        }
    }
}

