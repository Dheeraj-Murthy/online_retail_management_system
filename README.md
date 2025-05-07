## **1. Design Document**

### **1.1 System Architecture**

**Components**:

1. **Frontend**: HTML/CSS/JavaScript (Customer & Admin Interfaces)
2. **Backend**: C CGI Scripts (Linux Systems Programming)
3. **Database**: MySQL
4. **Web Server**: Apache HTTP Server

**Data Flow**:

```
Browser → Apache → CGI Scripts ↔ MySQL ↔ File System
```

### **1.2 Database Design**

**ER Diagram**:  
![ER Diagram](https://via.placeholder.com/800x400.png?text=ER+Diagram)

**Tables**:

```sql
1. Users (user_id, username, password_hash, role)
2. Products (product_id, name, price, stock)
3. Orders (order_id, user_id, status, total, timestamp)
4. OrderItems (order_id, product_id, quantity)
```

### **1.3 Security Design**

- SHA-256 password hashing
- Prepared SQL statements
- Session management via cookies
- Apache HTTPS configuration

---

## **2. User Manual**

### **2.1 Admin Guide**

**Dashboard Access**:  
`http://localhost/os_mini/admin.html`

**Features**:

1. **Add Product**:

   - Fill product details
   - Click "Add Product"

2. **Manage Inventory**:

   - View current stock
   - Auto-updates on product addition

3. **View Orders**:
   - Track order status (placed/paid/shipped/delivered)

### **2.2 Customer Guide**

**Store Access**:  
`http://localhost/os_mini/index.html`

**Features**:

1. **Browse Products**:

   - Horizontal scroll through products
   - Real-time stock display

2. **Shopping Cart**:

   - Click cart icon (top-right)
   - Modify quantities
   - Proceed to checkout

3. **Order Tracking**:
   - Order confirmation after checkout
   - Email notifications (simulated)

---

## **3. Installation Guide**

### **3.1 Prerequisites**

- macOS (Tested on Sonoma)
- Homebrew package manager
- Terminal access

### **3.2 Setup Instructions**

**1. Install Dependencies**:

```bash
brew update
brew install mysql httpd openssl
```

**2. Configure Apache**:

```bash
# Edit Apache config
nano /opt/homebrew/etc/httpd/httpd.conf

# Enable these lines:
LoadModule cgi_module lib/httpd/modules/mod_cgi.so
ScriptAlias /cgi-bin/ "/opt/homebrew/var/www/cgi-bin/"
```

**3. Set Up MySQL**:

```bash
brew services start mysql
mysql_secure_installation
```

**4. Deploy Project**:

```bash
# Database setup
mysql -u root -p < schema.sql

# Frontend files
cp -r frontend/* /opt/homebrew/var/www/htdocs/os_mini/

# Compile & deploy CGI scripts
gcc product.c -o product.cgi -lmysqlclient
chmod +x *.cgi
mv *.cgi /opt/homebrew/var/www/cgi-bin/os_mini/
```

**5. Configure Permissions**:

```bash
sudo chown -R _www /opt/homebrew/var/www/
sudo chmod -R 755 /opt/homebrew/var/www/
```

**6. Start Services**:

```bash
brew services restart httpd
brew services restart mysql
```

### **3.3 Post-Installation**

**Verify Installation**:

1. Access store: `http://localhost/os_mini`
2. Admin login: `admin/admin123`

---

## **4. Maintenance Guide**

### **4.1 Backup Database**

```bash
mysqldump -u root -p store_db > backup.sql
```

### **4.2 Update Stock Manually**

```sql
UPDATE Products SET stock = 100 WHERE product_id = 1;
```

### **4.3 View Logs**

```bash
# Apache Error Log
tail -f /opt/homebrew/var/log/httpd/error_log

# MySQL Log
tail -f /usr/local/var/mysql/$(hostname).err
```

---

## **5. Troubleshooting**

| **Issue**               | **Solution**                      |
| ----------------------- | --------------------------------- |
| 403 Forbidden           | Check file permissions            |
| 500 Internal Server     | Verify CGI script permissions     |
| MySQL Connection Failed | Verify credentials in C code      |
| Products Not Displaying | Check product.cgi output directly |
| Cart Not Saving         | Clear browser cache/localStorage  |

---

**Support Contact**: ms.dheerajmurthy@iiitb.ac.in  
**Version**: 1.0.0  
**Last Updated**: 2024-05-04

---
