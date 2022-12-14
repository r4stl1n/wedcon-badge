function getScriptStepLength() {
    var text = document.getElementById('ledscript').value;
    var lines = text.split("\n");
    return lines.length - 1;
}

function addLedStep() {
    const textarea = document.getElementById('ledscript');
    var dura = document.getElementById("duration").value;
    var ledArrays = [
        rgbToHSV(hexToRGB(document.getElementById("led1").value)),
        rgbToHSV(hexToRGB(document.getElementById("led2").value)),
        rgbToHSV(hexToRGB(document.getElementById("led3").value)),
        rgbToHSV(hexToRGB(document.getElementById("led4").value)),
        rgbToHSV(hexToRGB(document.getElementById("led5").value)),
        rgbToHSV(hexToRGB(document.getElementById("led6").value))
    ]
    var newString = dura
    for (const element of ledArrays) {
        newString += " " + element[0] + " " + element[1] + " " + element[2]
    }
    textarea.value += newString + "\n";

    Toastify({
        text: "Added step #" + getScriptStepLength(),
        duration: 1000,
        destination: "https://github.com/apvarun/toastify-js",
        newWindow: false,
        close: true,
        gravity: "top", // `top` or `bottom`
        position: "right", // `left`, `center` or `right`
        stopOnFocus: true, // Prevents dismissing of toast on hover
        style: {
            background: "#5755d9",
        },
        onClick: function() {} // Callback after click
    }).showToast();
}

function resetScript() {
    document.getElementById('ledscript').value = "";
    loadScriptFromDisk();

    if (document.getElementById('ledscript').value.length == 0) {
        loadDefaultScriptFromDisk();
    }

    Toastify({
        text: "Reset script",
        duration: 1000,
        destination: "https://github.com/apvarun/toastify-js",
        newWindow: false,
        close: true,
        gravity: "top", // `top` or `bottom`
        position: "right", // `left`, `center` or `right`
        stopOnFocus: true, // Prevents dismissing of toast on hover
        style: {
            background: "#e85600",
        },
        onClick: function() {} // Callback after click
    }).showToast();
}

function defaultScript() {
    document.getElementById('ledscript').value = "";
    loadDefaultScriptFromDisk();

    Toastify({
        text: "Load default script",
        duration: 1000,
        destination: "https://github.com/apvarun/toastify-js",
        newWindow: false,
        close: true,
        gravity: "top", // `top` or `bottom`
        position: "right", // `left`, `center` or `right`
        stopOnFocus: true, // Prevents dismissing of toast on hover
        style: {
            background: "#e85600",
        },
        onClick: function() {} // Callback after click
    }).showToast();
}

function saveScript() {
    var text = document.getElementById('ledscript').value;
    var lines = text.split("\n");

    var count = 1;
    for (const line of lines) {

        if (line == "") continue;

        count = count + 1;
        if (line.split(" ").length != 19) {

            Toastify({
                text: "Error on line: " + count,
                duration: 1000,
                destination: "https://github.com/apvarun/toastify-js",
                newWindow: false,
                close: true,
                gravity: "top", // `top` or `bottom`
                position: "right", // `left`, `center` or `right`
                stopOnFocus: true, // Prevents dismissing of toast on hover
                style: {
                    background: "#e85600",
                },
                onClick: function() {} // Callback after click
            }).showToast();

            return
        }
    }

    var details = {
        'script': document.getElementById('ledscript').value,
    };

    var formBody = [];
    for (var property in details) {
        var encodedKey = encodeURIComponent(property);
        var encodedValue = encodeURIComponent(details[property]);
        formBody.push(encodedKey + "=" + encodedValue);
    }
    formBody = formBody.join("&");

    fetch("/save", {
        method: "POST",
        headers: {
            'Content-Type': 'application/x-www-form-urlencoded'
        },
        body: formBody
    }).then(res => {
        Toastify({
            text: "Saved Script",
            duration: 1000,
            destination: "https://github.com/apvarun/toastify-js",
            newWindow: false,
            close: true,
            gravity: "top", // `top` or `bottom`
            position: "right", // `left`, `center` or `right`
            stopOnFocus: true, // Prevents dismissing of toast on hover
            style: {
                background: "#32b643",
            },
            onClick: function() {} // Callback after click
        }).showToast();
    });
}

function updateDuration() {
    var label = "Duration: " + document.getElementById('duration-slider').value + " Milliseconds";
    document.getElementById('duration-label').innerHTML = label;
    document.getElementById("duration").value = document.getElementById('duration-slider').value;

}

function loadDefaultScriptFromDisk() {
    const url = "/default-script.txt";
    fetch(url)
        .then(r => r.text())
        .then(t => document.getElementById('ledscript').value = t);

}

function loadScriptFromDisk() {
    const url = "/custom-script.txt";
    fetch(url)
        .then(r => r.text())
        .then(t => document.getElementById('ledscript').value = t);

}