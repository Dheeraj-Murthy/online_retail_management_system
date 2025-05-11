#include <mysql/mysql.h>
#include <openssl/sha.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void hash_password(const char *password, char *output) {
  unsigned char hash[SHA256_DIGEST_LENGTH];
  SHA256((unsigned char *)password, strlen(password), hash);
  for (int i = 0; i < SHA256_DIGEST_LENGTH; i++)
    sprintf(output + (i * 2), "%02x", hash[i]);
  output[64] = '\0';
}

int main() {
  // Read form data
  char *content_len = getenv("CONTENT_LENGTH");
  int len = content_len ? atoi(content_len) : 0;
  char data[256];
  fread(data, 1, len, stdin);
  data[len] = '\0';

  // Parse credentials
  char username[50], password[50];
  sscanf(data, "username=%49[^&]&password=%49s", username, password);

  // Hash password
  char hashed[65];
  hash_password(password, hashed);

  // Database connection
  MYSQL *conn = mysql_init(NULL);
  if (!mysql_real_connect(conn, "localhost", "root", "dheeraj", "store_db", 0,
                          NULL, 0)) {
    printf("Content-Type: text/plain\n\nDB Error");
    return 1;
  }

  // Insert user
  char query[256];
  snprintf(query, sizeof(query),
           "INSERT INTO users (username, password_hash) VALUES ('%s', '%s')",
           username, hashed);

  printf("Content-Type: text/plain\n\n");
  if (mysql_query(conn, query)) {
    printf("Username already exists");
  } else {
    printf("success");
  }

  mysql_close(conn);
  return 0;
}
