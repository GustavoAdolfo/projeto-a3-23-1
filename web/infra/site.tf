resource "aws_s3_object" "html" {
  for_each     = fileset("../site/build/", "**/*.html")
  bucket       = aws_s3_bucket.website.id
  key          = each.value
  source       = "../site/build/${each.value}"
  etag         = filemd5("../site/build/${each.value}")
  content_type = "text/html"
}

resource "aws_s3_object" "json" {
  for_each     = fileset("../site/build/", "**/*.json")
  bucket       = aws_s3_bucket.website.id
  key          = each.value
  source       = "../site/build/${each.value}"
  etag         = filemd5("../site/build/${each.value}")
  content_type = "application/json"
}

resource "aws_s3_object" "text" {
  for_each     = fileset("../site/build/", "**/*.txt")
  bucket       = aws_s3_bucket.website.id
  key          = each.value
  source       = "../site/build/${each.value}"
  etag         = filemd5("../site/build/${each.value}")
  content_type = "text/plan"
}

resource "aws_s3_object" "css" {
  for_each     = fileset("../site/build/", "**/*.css")
  bucket       = aws_s3_bucket.website.id
  key          = each.value
  source       = "../site/build/${each.value}"
  etag         = filemd5("../site/build/${each.value}")
  content_type = "text/css"
}

resource "aws_s3_object" "js" {
  for_each     = fileset("../site/build/", "**/*.{js,map}")
  bucket       = aws_s3_bucket.website.id
  key          = each.value
  source       = "../site/build/${each.value}"
  etag         = filemd5("../site/build/${each.value}")
  content_type = "application/javascript"
}

resource "aws_s3_object" "ico" {
  for_each     = fileset("../site/build/", "**/*.ico")
  bucket       = aws_s3_bucket.website.id
  key          = each.value
  source       = "../site/build/${each.value}"
  etag         = filemd5("../site/build/${each.value}")
  content_type = "image/ico"
}

resource "aws_s3_object" "png" {
  for_each     = fileset("../site/build/", "**/*.png")
  bucket       = aws_s3_bucket.website.id
  key          = each.value
  source       = "../site/build/${each.value}"
  etag         = filemd5("../site/build/${each.value}")
  content_type = "image/png"
}

resource "aws_s3_object" "svg" {
  for_each     = fileset("../site/build/", "**/*.svg")
  bucket       = aws_s3_bucket.website.id
  key          = each.value
  source       = "../site/build/${each.value}"
  etag         = filemd5("../site/build/${each.value}")
  content_type = "image/svg+xml"
}

resource "aws_s3_object" "jpg" {
  for_each     = fileset("../site/build/", "**/*.{jpg,jpeg,jfif,pjpeg,pjp}")
  bucket       = aws_s3_bucket.website.id
  key          = each.value
  source       = "../site/build/${each.value}"
  etag         = filemd5("../site/build/${each.value}")
  content_type = "image/jpg"
}
