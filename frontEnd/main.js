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

