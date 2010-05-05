/*These functions are used to get a "watermark" effect on a textbox.
 *Default text that describes what is expected to be entered is displayed and
 *the textbox's css class is set to "watermark" if there is nothing entered in it
 */
function watermark_onfocus(textbox) {
    if(textbox.className == 'watermark') {
        textbox.select();
        textbox.className = '';
    }
}

function watermark_onblur(textbox, watermark_value) {
    if(textbox.value == '' || textbox.value == watermark_value) {
        textbox.value = watermark_value;
        textbox.className = 'watermark';
    }
}

//This fixes Chrome behavior that deselects textbox content when the mouse goes up
function watermark_onmouseup(e) {
    e.preventDefault();
}