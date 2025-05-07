let cart = JSON.parse(localStorage.getItem('cart')) || [];

// ========== Cart Functions ========== //
function updateCartCount() {
  const totalItems = cart.reduce((sum, item) => sum + item.quantity, 0);
  document.getElementById('cart-count').textContent = totalItems;
}

function viewCart() {
  const modal = document.getElementById('cart-modal');
  const cartItems = document.getElementById('cart-items');

  cartItems.innerHTML = cart.map(item => `
    <div class="cart-item">
      ${item.name} - $${item.price.toFixed(2)} x ${item.quantity}
    </div>
  `).join('');

  modal.style.display = 'block';
}

function closeCart() {
  document.getElementById('cart-modal').style.display = 'none';
}

async function checkout() {
  try {
    const response = await fetch('/cgi-bin/os_mini/orders.cgi', {
      method: 'POST',
      body: JSON.stringify({ cart }),
      headers: { 'Content-Type': 'application/json' }
    });

    if (response.ok) {
      cart = [];
      localStorage.setItem('cart', JSON.stringify(cart));
      closeCart();
      loadProducts();  // Refresh stock display
      updateCartCount();
      alert('Order placed successfully!');
    }
  } catch (error) {
    console.error('Checkout failed:', error);
    alert('Checkout failed. Please try again.');
  }
}

// ========== Product Functions ========== //
async function loadProducts() {
  try {
    const response = await fetch('/cgi-bin/os_mini/product.cgi');
    const data = await response.text();
    document.getElementById('products').innerHTML = data;
    addAddToCartButtons();
  } catch (error) {
    console.error('Failed to load products:', error);
  }
}

function addAddToCartButtons() {
  document.querySelectorAll('.add-to-cart').forEach(button => {
    button.addEventListener('click', async (e) => {
      const productCard = e.target.closest('.product-card');
      await addToCart(productCard.dataset.id);
    });
  });
}

// ========== Core Cart Logic ========== //
async function addToCart(productId) {
  const productCard = document.querySelector(`[data-id="${productId}"]`);
  const currentStock = parseInt(productCard.dataset.stock);

  if (currentStock <= 0) {
    alert('Out of stock!');
    return;
  }

  try {
    // Update stock in backend
    const stockResponse = await fetch(`/cgi-bin/os_mini/update_stock.cgi?product_id=${productId}&quantity=1`);

    if (stockResponse.ok) {
      // Update frontend stock display
      const newStock = currentStock - 1;
      productCard.dataset.stock = newStock;
      productCard.querySelector('.stock-value').textContent = newStock;

      if (newStock === 0) {
        productCard.querySelector('.add-to-cart').disabled = true;
      }

      // Update cart
      const productName = productCard.querySelector('.product-name').textContent;
      const price = parseFloat(productCard.querySelector('.product-price').textContent.replace('$', ''));

      const existingItem = cart.find(item => item.id === productId);
      if (existingItem) {
        existingItem.quantity += 1;
      } else {
        cart.push({
          id: productId,
          name: productName,
          price: price,
          quantity: 1
        });
      }

      localStorage.setItem('cart', JSON.stringify(cart));
      updateCartCount();  // ← THIS IS CRUCIAL
    }
  } catch (error) {
    console.error('Add to cart failed:', error);
    alert('Failed to add to cart. Please try again.');
  }
}

// ========== Initialize ========== //
document.addEventListener('DOMContentLoaded', () => {
  loadProducts();
  updateCartCount();  // Initial cart count
});
