#include <mysql/mysql.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
  MYSQL *conn = mysql_init(NULL);
  mysql_real_connect(conn, "localhost", "root", "dheeraj", "store_db", 0, NULL,
                     0);

  // Handle POST requests (Admin adding products)
  char *method = getenv("REQUEST_METHOD");
  if (strcmp(method, "POST") == 0) {
    char data[1024];
    fgets(data, sizeof(data), stdin);
    char name[100], price[10], stock[10];
    sscanf(data, "name=%[^&]&price=%[^&]&stock=%s", name, price, stock);

    char query[256];
    snprintf(query, sizeof(query),
             "INSERT INTO Products (name, price, stock) VALUES ('%s', %s, %s)",
             name, price, stock);
    mysql_query(conn, query);
  }

  // Get all products
  mysql_query(conn, "SELECT * FROM Products");
  MYSQL_RES *result = mysql_store_result(conn);
  MYSQL_ROW row;

  printf("Content-Type: text/html\n\n");
  printf("<div class='product-grid'>");

  while ((row = mysql_fetch_row(result))) {
    printf("<div class='product-card' data-id='%s' data-stock='%s'>", row[0],
           row[3]);
    printf("<h3 class='product-name'>%s</h3>", row[1]);
    printf("<p class='product-price'>$%s</p>", row[2]);
    printf("<p class='stock'>Stock: <span class='stock-value'>%s</span></p>",
           row[3]);
    printf("<button class='add-to-cart' %s>Add to Cart</button>",
           atoi(row[3]) <= 0 ? "disabled" : "");
    printf("</div>");
  }

  printf("</div>");

  mysql_free_result(result);
  mysql_close(conn);
  return 0;
}
