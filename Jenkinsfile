pipeline {
    agent any
    
    // Définir les variables globales
    environment {
        DOCKER_REGISTRY = 'docker.io'
        DOCKER_USERNAME = 'safwenbenmabrouk' // Votre nom d'utilisateur Docker Hub
        REPO_NAME = 'tp-devOps'
        IMAGE_SERVER = "${DOCKER_USERNAME}/${REPO_NAME}-serveur"
        IMAGE_CLIENT = "${DOCKER_USERNAME}/${REPO_NAME}-client"
    }

    stages {
        stage('Checkout Code') {
            steps {
                // Récupère le code depuis le SCM (défini dans la configuration du job Jenkins)
                checkout scm 
            }
        }
        
        stage('Build & Test') {
            steps {
                script {
                    // Les étapes de compilation GCC sont maintenant dans le Dockerfile (Stage 1)
                    echo "Compilation des binaires incluse dans l'étape Docker Build Multi-Stage."
                }
            }
        }
        
        stage('Docker Build Images') {
            steps {
                script {
                    def imageTag = "build-${env.BUILD_NUMBER}"
                    
                    // 1. Build du Serveur
                    sh "docker build -t ${IMAGE_SERVER}:${imageTag} ./serveur"
                    sh "docker tag ${IMAGE_SERVER}:${imageTag} ${IMAGE_SERVER}:latest"
                    
                    // 2. Build du Client
                    sh "docker build -t ${IMAGE_CLIENT}:${imageTag} ./client"
                    sh "docker tag ${IMAGE_CLIENT}:${imageTag} ${IMAGE_CLIENT}:latest"
                }
            }
        }
        
        stage('Docker Push to Registry') {
            steps {
                // IMPORTANT : Nécessite un Credential ID Jenkins pour votre Docker Hub (voir 5.1)
                withCredentials([usernamePassword(credentialsId: 'docker-hub-credentials', passwordVariable: 'DOCKER_PASSWORD', usernameVariable: 'DOCKER_USER')]) {
                    sh "echo \"${DOCKER_PASSWORD}\" | docker login -u ${DOCKER_USER} --password-stdin ${DOCKER_REGISTRY}"

                    def imageTag = "build-${env.BUILD_NUMBER}"

                    // Push les tags
                    sh "docker push ${IMAGE_SERVER}:${imageTag}"
                    sh "docker push ${IMAGE_CLIENT}:${imageTag}"
                    
                    // Push les tags latest
                    sh "docker push ${IMAGE_SERVER}:latest"
                    sh "docker push ${IMAGE_CLIENT}:latest"
                }
            }
        }

        stage('Deploy to Kubernetes') {
            steps {
                // Met à jour les fichiers de manifeste avec le bon tag d'image
                sh "sed -i 's|image: .*monapp-serveur:.*|image: ${IMAGE_SERVER}:${env.BUILD_NUMBER}|g' ci-cd-config/k8s-serveur-deployment.yaml"
                sh "sed -i 's|image: .*monapp-client:.*|image: ${IMAGE_CLIENT}:${env.BUILD_NUMBER}|g' ci-cd-config/k8s-client-deployment.yaml"
                
                // Applique les manifestes au cluster Minikube
                // NOTE: Ceci fonctionne car Jenkins accède au moteur Docker de l'hôte, et kubectl est supposé être accessible par l'utilisateur 'root' du conteneur Jenkins.
                sh "kubectl apply -f ci-cd-config/k8s-serveur-deployment.yaml"
                sh "kubectl apply -f ci-cd-config/k8s-client-deployment.yaml"
            }
        }
    }
}