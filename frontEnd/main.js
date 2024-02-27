//rain
let hrElement;
let counter = 200;
for (let i = 0; i < counter; i++) {
  hrElement = document.createElement("HR");
  
	hrElement.style.left = Math.floor(Math.random() * window.innerWidth) + "px";
	hrElement.style.animationDuration = 0.8 + Math.random() * 4 + "s";
	hrElement.style.animationDelay = Math.random() * 5 + "s";
 
  document.body.appendChild(hrElement);
}

function directToCookies() {
	// Use window.location.href to set the new URL
	window.location.href = './cookies.html';
  }

function directToCGI() {
	// Use window.location.href to set the new URL
	window.location.href = './cgi.html';
}

function directToLibrary() {
	// Use window.location.href to set the new URL
	window.location.href = './library.html';
  }

function redirectHome() {
	// Use window.location.href to set the new URL
	window.location.href = './index.html';
}