/*
- Ignorando aws-s3-enable-versioning porque o bucket destina-se a ser acessao apenas pelo CloudFront
  e seu conteúdo atualizado pelo deploy da pipeline.
- Ignorando aws-s3-enable-bucket-logging porque o bucket destina-se a ser acessao apenas pelo CloudFront.
*/

#tfsec:ignore:aws-s3-enable-versioning
#tfsec:ignore:aws-s3-enable-bucket-logging
#tfsec:ignore:aws-s3-enable-bucket-encryption:Temporariamente para testes
#tfsec:ignore:aws-s3-encryption-customer-key:Temporariamente para testes
resource "aws_s3_bucket" "website" {
  #checkov:skip=CKV_AWS_21: Arquivos atualizados e versionados por controle de versão
  #checkov:skip=CKV_AWS_18: Não identificada a necessidade de log de acesso a arquivos neste caso
  #checkov:skip=CKV_AWS_144: Desabilitando replicação
  #checkov:skip=CKV2_AWS_61: Não se aplica
  #checkov:skip=CKV2_AWS_62: Não se aplica
  #checkov:skip=CKV_AWS_145: CloudFront não se comunica com encriptação dessa forma
  bucket = "a3-website"
}

resource "aws_s3_bucket_ownership_controls" "website" {
  bucket = aws_s3_bucket.website.id
  rule {
    object_ownership = "BucketOwnerPreferred"
  }
}

#tfsec:ignore:aws-s3-enable-versioning
resource "aws_s3_bucket_versioning" "website" {
  bucket = aws_s3_bucket.website.id
  versioning_configuration {
    status = "Disabled"
  }
}

resource "aws_s3_bucket_acl" "website" {
  depends_on = [aws_s3_bucket_ownership_controls.website]
  bucket     = aws_s3_bucket.website.bucket
  acl        = "private"
}

resource "aws_s3_bucket_public_access_block" "website" {
  bucket                  = aws_s3_bucket.website.id
  block_public_acls       = true
  block_public_policy     = true
  ignore_public_acls      = true
  restrict_public_buckets = true
}
