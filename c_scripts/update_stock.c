#include <mysql/mysql.h>
#include <stdio.h>
#include <stdlib.h>

int main() {
  MYSQL *conn = mysql_init(NULL);
  mysql_real_connect(conn, "localhost", "root", "dheeraj", "store_db", 0, NULL,
                     0);

  char *data = getenv("QUERY_STRING");
  int product_id, quantity = 1;
  sscanf(data, "product_id=%d&quantity=%d", &product_id, &quantity);

  char query[100];
  snprintf(query, sizeof(query),
           "UPDATE Products SET stock = stock - %d WHERE product_id = %d",
           quantity, product_id);
  mysql_query(conn, query);

  printf("Content-Type: text/html\n\n");
  printf("OK"); // Simplified response

  mysql_close(conn);
  return 0;
}
