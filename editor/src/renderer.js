
const { ipcRenderer } = require("electron");
const { exec, execSync } = require('child_process');
const fs = require("fs")
const keywords = "int float bool string void fn if else ret var with".split(" ");
const operators = "+ - * / % = == != < > <= >= && || !".split(" ");
const delimiters = " ( ) { } [ ] , ;".split(" ");
const openDelimiters = " ( { [".split(" ");
const closeDelimiters = " ) } ]".split(" ");
const literals = "true false".split(" ");


const delimiterMap = {
    "(": ")",
    "{": "}",
    "[": "]",
}   
var fontSize = 16;

const highlightedCode = document.getElementById('highlighted-code');

const editor = document.getElementById('editor');

editor.addEventListener('input', updateSuggestionHover);
document.addEventListener('click', updateSuggestionHover);
editor.addEventListener('keyup', updateSuggestionHover);


var curSuggestion = "";
var holdingControl = false;

var sampleProgram = `var void fizzbuzz = void <int n, int j> { // The function fizzbuzz returns void and takes two integer functions n and j as arguments
    if (j % 15 == 0) {
        log with (string <> { ret "FizzBuzz" })
    }
    else if (j % 5 == 0){
        log with (string <> { ret "Buzz" })
    }
    else if (j % 3 == 0){
        log with (string <> { ret "Fizz" })
    }
    else {
        log with (int <> { ret ^j })
    }
    if (j < n) {
        ^fizzbuzz with (int <> { ret ^n } , int <> { ret ^j + 1 }) // Recursive call
    }
}
fizzbuzz with (int <> { ret 100 }, int <> { ret 0 }) // Call the function with n = 100 and j = 0
`
window.addEventListener("DOMContentLoaded", (e)=>{
    editor.value = sampleProgram;
    updateFontSize()
    highlightedCode.innerHTML = highlight(editor.value);
    updateSuggestionHover()
    suggest()
})

editor.addEventListener('input', (event) => {

    if (openDelimiters.includes(event.data)) {
        const start = editor.selectionStart;

        let cdAhead = delimiterMap[event.data] == editor.value[start]
        let odBehind = openDelimiters.includes(editor.value[start - 2])
        console.log(cdAhead, editor.value[start], odBehind, editor.value[start - 1])
        if (!cdAhead || (cdAhead && odBehind)) 
            editor.value = editor.value.substring(0, start) + closeDelimiters[openDelimiters.indexOf(event.data)] + editor.value.substring(start);
        editor.selectionStart = start;
        editor.selectionEnd = start;
    }
    highlightedCode.innerHTML = highlight(editor.value);
    curSuggestion = suggest();
    document.querySelector("#hover-box").innerHTML = highlight(curSuggestion);
});
var setScroll = () => {
    highlightedCode.scrollTop = editor.scrollTop;
    highlightedCode.scrollLeft = editor.scrollLeft;
}
editor.addEventListener('scroll', setScroll);

function updateSuggestionHover(){
    let pos = getCaretPosition(editor);
    let suggestionOffset = {
        left: 0, 
        top: fontSize + 5,
    }
    const elem = document.querySelector("#hover-box")
    elem.style.top = `${pos.top + suggestionOffset.top}px`;
    elem.style.left = `${pos.left + suggestionOffset.left}px`;
}

function removeLastWord(code, cursor) {
    let i = cursor - 1;
    while (i >= 0) {
        if (code[i] == " " || code[i] == "\n" || delimiters.includes(code[i]) || operators.includes(code[i])) {
            break;
        }
        i--;
    }
    return code.substring(0, i + 1);
}
function updateFontSize(){
    editor.style.fontSize = fontSize + "px";
    highlightedCode.style.fontSize = fontSize + "px";
    document.querySelector("#hover-box").style.fontSize = fontSize + "px";
}
editor.addEventListener("keydown", (e) => {
    if (!file_modified && file_lastLoad != null){ 
        const qp = document.getElementById("quicksave-path");
        qp.innerText += " ●";
        file_modified = true;
    }

    
    if (e.key == "Tab") {
        e.preventDefault();
        const start = e.target.selectionStart;
        const end = e.target.selectionEnd;
        const selection = e.target.value.substring(start, end);
        const tab = curSuggestion == "" ? "    " : curSuggestion;
        if (curSuggestion != "") {
            e.target.value = removeLastWord(e.target.value, start) + tab + e.target.value.substring(end);
            e.target.selectionStart = start + tab.length;
            e.target.selectionEnd = start;
        }
        else {
            e.target.value = e.target.value.substring(0, start) + tab + selection + e.target.value.substring(end);
            e.target.selectionStart = start + tab.length;
            e.target.selectionEnd = start
        }
        // Set cursor position
        editor.focus();
        editor.setSelectionRange(start + tab.length, start + tab.length);
    }
    else if (holdingControl){
        // Up arrow
        if (e.key == "ArrowUp"){
            fontSize += 1;
            updateFontSize()
            return;
        }
        // Down arrow
        else if (e.key == "ArrowDown"){
            fontSize -= 1;
            updateFontSize()
            return;
        }
        else if (e.key.toLowerCase() == "s"){
            saveFile(true)
        }
    }
    else if (e.key == "Control") {
        holdingControl = true;
    }
    updateSuggestionHover()
    highlightedCode.innerHTML = highlight(editor.value);
});

editor.addEventListener("keyup", (e) => {
    if (e.key == "Control") {
        holdingControl = false;
    }
    highlightedCode.innerHTML = highlight(editor.value);
});


function currentWord(code, cursor) {
    let word = "";
    let i = cursor - 1;
    while (i >= 0) {
        if (code[i] == " " || code[i] == "\n" || delimiters.includes(code[i]) || operators.includes(code[i])) {
            break;
        }
        word = code[i] + word;
        i--;
    }
    return word;
}

function closestTo(word, targets){
    let closest = 0;
    let cWord = "";
    if (word.length == 0) return ""
    for (let i = 0; i < targets.length; i++){
        w = targets[i];
        c = 0;
        if (word.length >= w.length){
            if (word[word.length - 1] != w[w.length - 1]) {
                c = -Infinity;
                continue;
            }
        }
        let errors = 0;
        for (let j = 0; j < Math.min(word.length, w.length); j++){
            if(w[j] == word[j]) c+=2;
            else errors++;
        }
        c = c - Math.abs(word.length - w.length) - errors*errors
        if (c > closest){
            closest = c;
            cWord = w;
            console.log("Found: ", cWord, c);
        }
    }
    return cWord;
}

function suggest() {
    const code = editor.value;
    const cursor = editor.selectionStart;
    const cw = currentWord(code, cursor);
    //console.log("Current word: ", cw, "Cursor: ", cursor, "Code: ", code);
    
    const c = closestTo(cw, 
        keywords
        .concat(literals)
    );
    if (c.length == 0){
        document.querySelector("#hover-box").style.display = 'none';
    }else{
        document.querySelector("#hover-box").style.display = 'flex';
    }
    return c 
}
function highlightWord(word){
    if (/\d+/.test(word)) {
        return `<span class="number">${word}</span>`;
    } else if (/\d+\.\d+/.test(word)) {
        return `<span class="number">${word}</span>`;
    } else if (keywords.includes(word)) {
        return `<span class="keyword">${word}</span>`;
    } else if (operators.includes(word)) {
        const replace = {
            ">": "&gt;",
            "<": "&lt;",
            "&&": "&amp;&amp;",
            "||": "&#124;&#124;",
            "!": "&#33;",
            "==": "&#61;&#61;",
            "!=": "&#33;&#61;",
        }
        if (word in replace){
            return `<span class="operator">${replace[word]}</span>`;
        }
        return `<span class="operator">${word}</span>`;
    } else if (delimiters.includes(word)) {
        return `<span class="delimiter">${word}</span>`;
    } else if (literals.includes(word)) {
        return `<span class="literal">${word}</span>`;
    } else {
        return word;
    }
}
let DP = {}
function highlightLine(line) {

    if (line in DP) 
        return DP[line];

    let highlightedLine = "";
    let word = "";
    let inWord = false;
    for (let i = 0; i < line.length; i++) {
        // "hello"
        // iter2, i = 7
        const char = line[i]; // "
        if (char == "/"){
            if (line[i+1] == "/"){
                highlightedLine += `<span class="comment">${line.substring(i)}</span>`
                break;
            }
        }
        if (char == "\""){
            if(word.length != 0){
                highlightedLine += highlightWord(word);
                word = "";
            }
            let j = i+1;

            let cstr = "\""
            while (line[j] != "\"" && j < line.length){
                cstr += line[j]
                j++;
            }
            // cstr = "hello
            i = j; // i = 7
            cstr += line[j] == "\"" ? "\"" : "" // adds 
            highlightedLine += `<span class="string">${cstr}</span>` // <span class="string">"hello"</span>
        }
        else if (operators.includes(char)) {
            if (inWord) {
                inWord = false;
                highlightedLine += highlightWord(word);
                word = "";
            }
            highlightedLine += highlightWord(char);
        }
        else if (char == " " || char == "\n" || delimiters.includes(char)) {
            if (inWord) {
                inWord = false;
                highlightedLine += highlightWord(word);
                word = "";
            }
            highlightedLine += char;
        } else {
            if (!inWord) {
                inWord = true;
            }
            word += char;
        }
    }
    if (inWord) {
        highlightedLine += highlightWord(word);
    }
    DP[line] = highlightedLine;
    return highlightedLine;
}
function highlight(code) {
    let highlightedCode = "";
    const lines = code.split("\n");
    for (let i = 0; i < lines.length; i++) {
        highlightedCode += highlightLine(lines[i]);
        if (i != lines.length - 1) {
            highlightedCode += "\n";
        }
    }
    setScroll()
    return highlightedCode;

}
function setOutput(text, type){
    // Sanitize text
    let out = "";
    switch(type){
        case "text":
            out = text
        case "error":
            out = `<span class="error">${text}</span>`
    }
    document.querySelector("#output-box").innerHTML = out;
}
document.querySelector("#run-button").addEventListener("click", () => {
    const code = editor.value;
    fs.writeFileSync(__dirname + "/_tmp_lppcode.lpp", code);
    ipcRenderer.send('get-compiled-code');
    // Run the compiled code
    let output = "There was an error, abort!";
    const cmd = `node ${__dirname}/_tmp_lppcompiledcode.js`;
    exec(cmd, (error, stdout, stderr) => {
        if (error) {
            output = error.message;
            console.error(`Error: ${error.message}`);
        }
        else if (stderr) {
            output = stderr;
            console.error(`Stderr: ${stderr}`);
        }
        else {
            output = stdout.replace(/\n/g, "<br>");
            console.log(`Output: ${stdout}`);
        }
        document.querySelector("#output-box").innerHTML = output;
    });
    
});