const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <meta http-equiv="X-UA-Compatible" content="IE=edge">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <style>
    body {
      text-align: center;
    }
    li {
      list-style-type: none;
    }

    .holder {
      display: flex;
      justify-content: center;
      align-items: center;
    }
  </style>
  <script>
    function addListItem(value, idx) {
      let list = document.getElementById(`list_${idx}`)
      let entry = document.createElement('li')
      let input = document.createElement('input')
      input.type = 'text'
      input.value = value
      entry.appendChild(input)
      list.appendChild(entry)
    }

    function removeListItem(idx) {
      let ul = document.querySelector(`#list_${idx}`)
      let list = ul.querySelectorAll('li')
      console.log(list)
      let n = list.length - 1
      console.log(n)
      list[n].parentNode.removeChild(list[n])
    }
    
    function save() {
      let ul1 = document.querySelector('#list_1')
      let ul2 = document.querySelector('#list_2')
      let snl = ul1.querySelectorAll('li input')
      let dbl = ul2.querySelectorAll('li input')
      let out = {
        single: {
          list: [],
          size: 0
        },
        double: {
          list: [],
          size: 0
        }
      }

      out.single.size = snl.length
      out.double.size = dbl.length
      
      for (let i = 0; i < snl.length; i++) {
        out.single.list[i] = snl[i].value
      }
      for (let i = 0; i < dbl.length; i++) {
        out.double.list[i] = dbl[i].value
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

      for(let i = 0; i < data.single.size; i++) {
        addListItem(data.single.list[i], 1)
      }
      for(let i = 0; i < data.double.size; i++) {
        addListItem(data.double.list[i], 2)
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
  <div class="holder">
    <div class="block">
      <ul id="list_1">
      </ul>
      <button onclick="addListItem('192.168.0.', 1)">Add</button>
      <button onclick="removeListItem(1)">Remove</button>
    </div>
    <div class="block">
      <ul id="list_2">
      </ul>
      <button onclick="addListItem('192.168.0.', 2)">Add</button>
      <button onclick="removeListItem(2)">Remove</button>
    </div>
  </div>
  <button onclick="save()">Save</button>
</body>
</html>
)rawliteral";