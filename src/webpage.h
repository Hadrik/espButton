const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <meta http-equiv="X-UA-Compatible" content="IE=edge">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <style>
    body {
      align-items: center;
    }
    li {
      list-style-type: none;
    }
  </style>
  <script>
    function addListItem(value) {
      let list = document.getElementById('list')
      let entry = document.createElement('li')
      let input = document.createElement('input')
      input.type = 'text'
      input.value = value
      entry.appendChild(input)
      list.appendChild(entry)
    }

    function removeListItem() {
      let list = document.querySelectorAll('ul li')
      let n = list.length - 1
      list[n].parentNode.removeChild(list[n])
    }

    function save() {
      let a = document.querySelectorAll('ul li input')
      let out = {
        list: [],
        size: 0
      }
      out.size = a.length
      for (let i = 0; i < a.length; i++) {
        out.list[i] = a[i].value
      }

      fetch('/', {
        method: 'POST',
        headers: {
          'content-type': 'application/json'
        },
        body: JSON.stringify(out)
      })
    }

    async function getAndUpdateList() {
      let response = await fetch('/list')
      let data = await response.json()
      console.log(data)

      for(let i = 0; i < data.size; i++) {
        addListItem(data.list[i])
      }
    }

    getAndUpdateList()
  </script>
  <title>Edit</title>
</head>
<body>
  <h1>
    List of addresses
  </h1>
  <ul id="list">
  </ul>
  <button onclick="addListItem('192.168.0.')">Add</button>
  <button onclick="removeListItem()">Remove</button>
  <button onclick="save()">Save</button>
</body>
</html>
)rawliteral";