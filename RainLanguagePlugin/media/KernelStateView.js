


const stateChart = new Chart(document.getElementById('container'), {
    type: "line",
    data: {
        labels: [],
        datasets: [
            {
                label: "task",
                data: [],
                fill: false,
                borderColor: "rgb(255, 0, 0)",
                borderWidth: 1,
                tension: 0,
                pointRadius: 1
            },
            {
                label: "string",
                data: [],
                fill: false,
                borderColor: "rgb(0, 255, 0)",
                borderWidth: 1,
                tension: 0,
                pointRadius: 1
            },
            {
                label: "entity",
                data: [],
                fill: false,
                borderColor: "rgb(0, 0, 255)",
                borderWidth: 1,
                tension: 0,
                pointRadius: 1
            },
            {
                label: "handle",
                data: [],
                fill: false,
                borderColor: "rgb(0, 255, 255)",
                borderWidth: 1,
                tension: 0,
                pointRadius: 1
            },
            {
                label: "heap",
                data: [],
                fill: false,
                borderColor: "rgb(255, 255, 0)",
                borderWidth: 1,
                tension: 0,
                pointRadius: 1
            }
        ]
    },
    options: {
        animation: false
    }
});

window.addEventListener('message', event => {
    const msg = event.data
    const datasets = stateChart.data.datasets
    switch (msg.type) {
        case 'show': {
            stateChart.data.labels = []
            for (let index = 0; index < datasets.length; index++) {
                const element = datasets[index];
                element.data = []
            }
            stateChart.update('show')
            break
        }
        case 'data': {
            if (stateChart.data.labels.length == 100) {
                for (let index = 0; index < datasets.length; index++) {
                    const element = datasets[index];
                    element.data.shift()
                }
                stateChart.data.labels.shift()
            }
            const label = msg["label"]
            if (label) {
                stateChart.data.labels.push(label)
            } else {
                stateChart.data.labels.push("")
            }
            for (let index = 0; index < datasets.length; index++) {
                const element = datasets[index];
                element.data.push(msg.data[element.label])
            }
            stateChart.update()
        }
    }
})