pipeline {
    agent {
        label 'ZAPD'
    }

    stages {
        // Non-parallel ZAPD stage
        stage('Build ZAPD') {
            steps {
                sh 'make -j WERROR=1'
            }
        }

        // CHECKOUT THE REPOS
        stage('Checkout Repos') {
            parallel {
                stage('Checkout oot') {
                    steps {
                        dir('oot') {
                            git url: 'https://github.com/zeldaret/oot.git'
                        }
                    }
                }

                stage('Checkout mm') {
                    steps{
                        dir('mm') {
                            git url: 'https://github.com/zeldaret/mm.git'
                        }
                    }
                }
            }
        }

        // SETUP THE REPOS
        stage('Set up repos') {
            parallel {
                stage('Setup OOT') {
                    steps {
                        dir('oot') {
                            sh 'cp /usr/local/etc/roms/baserom_oot.z64 baserom_original.z64'

                            // Identical to `make setup` except for copying our newer ZAPD.out into oot
                            sh 'git submodule update --init --recursive'
                            sh 'make -C tools'
                            sh 'cp ../ZAPD.out tools/ZAPD/'
                            sh 'python3 fixbaserom.py'
                            sh 'python3 extract_baserom.py'
                            sh 'python3 extract_assets.py'
                        }
                    }
                }

                stage('Setup MM') {
                    steps {
                        dir('mm') {
                            sh 'cp /usr/local/etc/roms/mm.us.rev1.z64 baserom.mm.us.rev1.z64'

                            // Identical to `make setup` except for copying our newer ZAPD.out into mm
                            sh 'make -C tools'
                            sh 'cp ../ZAPD.out tools/ZAPD/'
                            sh 'python3 tools/fixbaserom.py'
                            sh 'python3 tools/extract_baserom.py'
                            sh 'python3 extract_assets.py -j $(nproc)'
                        }
                    }
                }
            }
        }

        // INSTALL PYTHON DEPENDENCIES, currently MM only
        stage('Install Python dependencies') {
            steps {
                dir('mm') {
                    sh 'python3 -m pip install -r requirements.txt'
                }
            }
        }

        // BUILD THE REPOS
        stage('Build repos') {
            parallel {
                stage('Build oot') {
                    steps {
                        dir('oot') {
                            sh 'make -j'
                        }
                    }
                }
                stage('Build mm') {
                    steps {
                        dir('mm') {
                            sh 'make -j disasm'
                            sh 'make -j'
                        }
                    }
                }
            }
        }
    }
    post {
        always {
            cleanWs()
        }
    }
}
