terraform {
  required_version = "1.4.6"
  required_providers {
    aws = {
      source  = "hashicorp/aws"
      version = "~> 4.61.0"
    }
  }
}

provider "aws" {
  region = "us-east-1"

  default_tags {
    tags = {
      Terraform = "True"
    }
  }
}

data "aws_caller_identity" "current" {}
data "aws_region" "current" {}
