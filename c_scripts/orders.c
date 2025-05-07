#include <mysql/mysql.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
  MYSQL *conn = mysql_init(NULL);
  mysql_real_connect(conn, "localhost", "root", "dheeraj", "store_db", 0, NULL,
                     0);

  // Read POST data
  char *content_length_str = getenv("CONTENT_LENGTH");
  int content_length = atoi(content_length_str);
  char *post_data = malloc(content_length + 1);
  fread(post_data, 1, content_length, stdin);
  post_data[content_length] = '\0';

  printf("Content-Type: text/html\n\n");

  // Start transaction
  mysql_query(conn, "START TRANSACTION");

  // Create new order
  mysql_query(
      conn,
      "INSERT INTO Orders (user_id, total, status) VALUES (1, 0, 'placed')");
  int order_id = mysql_insert_id(conn);

  // Parse cart items (simplified format:
  // product_id=1&quantity=2&product_id=3&quantity=1)
  char *token = strtok(post_data, "&");
  while (token != NULL) {
    int product_id, quantity;
    if (sscanf(token, "product_id=%d", &product_id)) {
      token = strtok(NULL, "&");
      if (token && sscanf(token, "quantity=%d", &quantity)) {
        // Insert order item
        char query[100];
        snprintf(query, sizeof(query),
                 "INSERT INTO OrderItems (order_id, product_id, quantity) "
                 "VALUES (%d, %d, %d)",
                 order_id, product_id, quantity);
        mysql_query(conn, query);
      }
    }
    token = strtok(NULL, "&");
  }

  // Commit transaction
  mysql_query(conn, "COMMIT");

  printf("Order %d created successfully!", order_id);

  free(post_data);
  mysql_close(conn);
  return 0;
}
