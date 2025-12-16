pipeline {
    agent any
    environment {
        DOCKER_REGISTRY = 'docker.io'
        DOCKER_USERNAME = 'safwenbenmabrouk'
        REPO_NAME = 'atelier-jenkins-k8s' 
        IMAGE_SERVER = "${DOCKER_USERNAME}/${REPO_NAME}-serveur"
        IMAGE_CLIENT = "${DOCKER_USERNAME}/${REPO_NAME}-client"
    }
    stages {
        stage('Checkout') {
            steps {
                checkout scm
            }
        }
        stage('Build Images') {
            steps {
                script {
                    def imageTag = "build-${env.BUILD_NUMBER}"
                    sh "docker build -t ${IMAGE_SERVER}:${imageTag} ./serveur"
                    sh "docker tag ${IMAGE_SERVER}:${imageTag} ${IMAGE_SERVER}:latest"
                    sh "docker build -t ${IMAGE_CLIENT}:${imageTag} ./client"
                    sh "docker tag ${IMAGE_CLIENT}:${imageTag} ${IMAGE_CLIENT}:latest"
                }
            }
        }
        stage('Push to Docker Hub') {
            steps {
                script {
                    def imageTag = "build-${env.BUILD_NUMBER}"
                    withCredentials([usernamePassword(credentialsId: 'dockerhub-creds', 
                                                    usernameVariable: 'DOCKER_USER',
                                                    passwordVariable: 'DOCKER_PASS')]) {
                        sh 'echo "$DOCKER_PASS" | docker login -u $DOCKER_USER --password-stdin $DOCKER_REGISTRY'
                        sh "docker push ${IMAGE_SERVER}:${imageTag}"
                        sh "docker push ${IMAGE_CLIENT}:${imageTag}"
                        sh "docker push ${IMAGE_SERVER}:latest"
                        sh "docker push ${IMAGE_CLIENT}:latest"
                    }
                }
            }
        }
    }
    post {
        always {
            sh "docker rmi ${IMAGE_SERVER}:build-${env.BUILD_NUMBER} || true"
            sh "docker rmi ${IMAGE_CLIENT}:build-${env.BUILD_NUMBER} || true"
            sh "docker rmi ${IMAGE_SERVER}:latest || true"
            sh "docker rmi ${IMAGE_CLIENT}:latest || true"
        }
    }
}