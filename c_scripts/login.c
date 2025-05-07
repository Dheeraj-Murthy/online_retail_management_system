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
  char *data = getenv("QUERY_STRING");
  char username[50] = {0};
  char password[50] = {0};

  // Parse input
  if (data) {
    sscanf(data, "username=%49[^&]&password=%49s", username, password);
  }

  // Hash password
  char hashed_password[65] = {0};
  hash_password(password, hashed_password);
  fprintf(stderr, "[DEBUG] Username: %s\n", username);
  fprintf(stderr, "[DEBUG] Hashed Password: %s\n", hashed_password);
  // Connect to MySQL
  MYSQL *conn = mysql_init(NULL);
  if (!mysql_real_connect(conn, "localhost", "root", "dheeraj", "store_db", 0,
                          NULL, 0)) {
    fprintf(stderr, "Database connection failed: %s\n", mysql_error(conn));
    printf("Content-Type: text/html\n\n");
    printf("Login Failed (DB Connection)");
    exit(1);
  }

  // Prepared statement
  MYSQL_STMT *stmt = mysql_stmt_init(conn);
  const char *query =
      "SELECT role FROM Users WHERE username=? AND password_hash=?";

  if (mysql_stmt_prepare(stmt, query, strlen(query))) {
    fprintf(stderr, "Prepare failed: %s\n", mysql_stmt_error(stmt));
    printf("Content-Type: text/html\n\n");
    printf("Login Failed (Prepare)");
    mysql_close(conn);
    exit(1);
  }

  // Bind parameters
  MYSQL_BIND params[2];
  memset(params, 0, sizeof(params));

  params[0].buffer_type = MYSQL_TYPE_STRING;
  params[0].buffer = username;
  params[0].buffer_length = strlen(username);

  params[1].buffer_type = MYSQL_TYPE_STRING;
  params[1].buffer = hashed_password;
  params[1].buffer_length = 64;

  if (mysql_stmt_bind_param(stmt, params)) {
    fprintf(stderr, "Bind param failed: %s\n", mysql_stmt_error(stmt));
    printf("Content-Type: text/html\n\n");
    printf("Login Failed (Bind)");
    mysql_stmt_close(stmt);
    mysql_close(conn);
    exit(1);
  }

  // Execute statement
  if (mysql_stmt_execute(stmt)) {
    fprintf(stderr, "Execute failed: %s\n", mysql_stmt_error(stmt));
    printf("Content-Type: text/html\n\n");
    printf("Login Failed (Execute)");
    mysql_stmt_close(stmt);
    mysql_close(conn);
    exit(1);
  }
  fprintf(stderr,
          "[DEBUG] Executing query: SELECT role FROM Users WHERE username='%s' "
          "AND password_hash='%s'\n",
          username, hashed_password);

  // Bind result
  char role[10] = {0}; // Initialize to zeros
  MYSQL_BIND result;
  memset(&result, 0, sizeof(result));
  result.buffer_type = MYSQL_TYPE_STRING;
  result.buffer = role;
  result.buffer_length = sizeof(role) - 1; // Leave space for null terminator

  if (mysql_stmt_bind_result(stmt, &result)) {
    fprintf(stderr, "Bind result failed: %s\n", mysql_stmt_error(stmt));
    printf("Content-Type: text/html\n\n");
    printf("Login Failed (Result Bind)");
    mysql_stmt_close(stmt);
    mysql_close(conn);
    exit(1);
  }

  // Store result and check if we have a row
  if (mysql_stmt_store_result(stmt)) {
    fprintf(stderr, "Store result failed: %s\n", mysql_stmt_error(stmt));
    printf("Content-Type: text/html\n\n");
    printf("Login Failed (Store Result)");
    mysql_stmt_close(stmt);
    mysql_close(conn);
    exit(1);
  }

  int result_code = mysql_stmt_fetch(stmt);
  printf("Content-Type: text/html\n\n");

  if (result_code == 0) {
    // Successful fetch
    printf("Success! Role: %s", role);
  } else if (result_code == MYSQL_NO_DATA) {
    printf("Login Failed! (No matching user)");
  } else {
    fprintf(stderr, "Fetch failed: %s\n", mysql_stmt_error(stmt));
    printf("Login Failed! (Error)");
  }

  mysql_stmt_free_result(stmt);
  mysql_stmt_close(stmt);
  mysql_close(conn);
  return 0;
}
