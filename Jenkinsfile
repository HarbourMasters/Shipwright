pipeline {

    environment {
        MSBUILD='C:\\Program Files\\Microsoft Visual Studio\\2022\\Community\\Msbuild\\Current\\Bin\\msbuild.exe'
        CONFIG='Release'
        OTRPLATFORM='x64'
        PLATFORM='x86'
        ZIP='C:\\Program Files\\7-Zip\\7z.exe'
        PYTHON='C:\\Users\\jenkins\\AppData\\Local\\Programs\\Python\\Python310\\python.exe'
        TOOLSET='v142'
        EMAILTO=''
    }

    agent {
        label 'SoH-Builders'
    }

    options {
        timestamps() 
        timeout(time: 15, unit: 'MINUTES')
        skipDefaultCheckout(true)
    }

    stages {

        stage ('Checkout') {
            steps {
                checkout([
                    $class: 'GitSCM',
                    branches: scm.branches,
                    doGenerateSubmoduleConfigurations: scm.doGenerateSubmoduleConfigurations,
                    extensions: scm.extensions,
                    userRemoteConfigs: scm.userRemoteConfigs
                ])
            }
        }

        stage ('Build OTRExporter') {
            steps {
                bat """ 
                
                "${env.MSBUILD}" ".\\OTRExporter\\OTRExporter.sln" -t:restore,build -p:Configuration=${env.CONFIG};Platform=${env.OTRPLATFORM};PlatformToolset=${env.TOOLSET};RestorePackagesConfig=true /nodeReuse:false
                
                """
            }
        }

        stage ('Extract assets') {
            steps {
                bat """ 

                xcopy "..\\..\\ZELOOTD.z64" "OTRExporter\\"

                cd "OTRExporter"
                "${env.PYTHON}" ".\\extract_assets.py"
                cd "${env.WORKSPACE}"

                """
            }
        }

        stage ('Build SoH') {
            steps {
                bat """

                "${env.MSBUILD}" ".\\soh\\soh.sln" -t:build -p:Configuration=${env.CONFIG};Platform=${env.PLATFORM};PlatformToolset=${env.TOOLSET} /nodeReuse:false

                """
            }
        }

        stage ('Archive artifacts') {
            steps {
                bat """

                "${env.ZIP}" a "soh.zip" ".\\soh\\Release\\soh.exe"

                """

                archiveArtifacts allowEmptyArchive: false, 
                artifacts: 'soh.zip', 
                caseSensitive: true, 
                defaultExcludes: true, 
                fingerprint: false, 
                onlyIfSuccessful: true
            }
        }
    }

    post {
        always {
            step([$class: 'Mailer',
            notifyEveryUnstableBuild: true,
            recipients: "${env.EMAILTO}",
            sendToIndividuals: false])
            step([$class: 'WsCleanup']) // Clean workspace
        }
    }
}
