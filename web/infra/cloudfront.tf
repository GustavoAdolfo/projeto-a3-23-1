resource "aws_cloudfront_origin_access_identity" "origin_access_identity" {
  comment = "Buckets do site e recursos"
}

data "aws_iam_policy_document" "s3_policy" {
  statement {
    actions   = ["s3:GetObject"]
    resources = ["${aws_s3_bucket.website.arn}/*"]

    principals {
      type        = "AWS"
      identifiers = [aws_cloudfront_origin_access_identity.origin_access_identity.iam_arn]
    }
  }

  statement {
    actions   = ["s3:PutObject"]
    resources = ["${aws_s3_bucket.website.arn}/*"]

    principals {
      type        = "AWS"
      identifiers = [aws_cloudfront_origin_access_identity.origin_access_identity.iam_arn]
    }
  }

  statement {
    actions   = ["s3:ListBucket"]
    resources = [aws_s3_bucket.website.arn]

    principals {
      type        = "AWS"
      identifiers = [aws_cloudfront_origin_access_identity.origin_access_identity.iam_arn]
    }
  }
}

resource "aws_s3_bucket_policy" "json" {
  bucket = aws_s3_bucket.website.id
  policy = data.aws_iam_policy_document.s3_policy.json
}

#tfsec:ignore:aws-cloudfront-enable-logging
#tfsec:ignore:aws-cloudfront-enable-waf
resource "aws_cloudfront_distribution" "website" {
  #checkov:skip=CKV_AWS_310: Anotado para implementações futuras.
  #checkov:skip=CKV2_AWS_47: Aparentemente redundante
  #checkov:skip=CKV_AWS_86
  #checkov:skip=CKV_AWS_68
  aliases             = ["projetoa3.${var.domain_name}"]
  enabled             = true
  comment             = "Projeto A3 2023.1"
  default_root_object = "index.html"
  is_ipv6_enabled     = true

  origin {
    domain_name = aws_s3_bucket.website.bucket_regional_domain_name
    origin_id   = "S3-www.${aws_s3_bucket.website.id}"
    s3_origin_config {
      origin_access_identity = aws_cloudfront_origin_access_identity.origin_access_identity.cloudfront_access_identity_path
    }
  }

  default_cache_behavior {
    target_origin_id           = "S3-www.${aws_s3_bucket.website.id}"
    allowed_methods            = ["DELETE", "GET", "HEAD", "OPTIONS", "PATCH", "POST", "PUT"]
    cached_methods             = ["GET", "HEAD"]
    min_ttl                    = 0
    default_ttl                = 900
    max_ttl                    = 5400
    viewer_protocol_policy     = "redirect-to-https"
    compress                   = true
    response_headers_policy_id = aws_cloudfront_response_headers_policy.headers.id
    forwarded_values {
      query_string = true
      cookies {
        forward = "all"
      }
    }
  }

  ordered_cache_behavior {
    path_pattern     = "/*"
    allowed_methods  = ["GET", "HEAD", "OPTIONS"]
    cached_methods   = ["GET", "HEAD", "OPTIONS"]
    target_origin_id = "S3-www.${aws_s3_bucket.website.id}"

    forwarded_values {
      query_string = false
      headers      = ["Origin"]

      cookies {
        forward = "none"
      }
    }

    min_ttl                = 0
    default_ttl            = 86400
    max_ttl                = 31536000
    compress               = true
    viewer_protocol_policy = "redirect-to-https"
  }


  custom_error_response {
    error_caching_min_ttl = 0
    error_code            = 404
    response_code         = 200
    response_page_path    = "/404.html"
  }

  restrictions {
    geo_restriction {
      restriction_type = "whitelist"
      locations        = ["BR"]
    }
  }

  viewer_certificate {
    acm_certificate_arn      = "arn:aws:acm:${data.aws_region.current.name}:${data.aws_caller_identity.current.account_id}:certificate/9d4be734-03ea-4d40-808e-bf1d24fcb497"
    ssl_support_method       = "sni-only"
    minimum_protocol_version = "TLSv1.2_2021"
  }

  # logging_config {
  #   include_cookies = false
  #   bucket          = var.bucket_cdn_log
  #   prefix          = "webapp"
  # }

  # web_acl_id = var.waf2_acl_arn
}

resource "aws_cloudfront_response_headers_policy" "headers" {
  name = "a3-headers-policy"

  security_headers_config {
    strict_transport_security {
      access_control_max_age_sec = 31536000
      include_subdomains         = true
      override                   = true
      preload                    = true
    }
  }

  custom_headers_config {

    items {
      header   = "Projeto-A3"
      override = true
      value    = "2023.1"
    }
  }
}
