var drawing = document.getElementById("drawing");

if (drawing.getContext) {
    var context = drawing.getContext("2d");
    
    context.beginPath();
    context.arc(100, 100, 99, 0, 2 * Math.PI, false);
    
    context.moveTo(194, 100);
    context.arc(100, 100, 94, 0, 2 * Math.PI, false);
    
    context.moveTo(100, 100);
    context.lineTo(100, 15);
    
    context.moveTo(100, 100);
    context.lineTo(35, 100);
    
    
    context.stroke();
}