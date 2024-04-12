


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
                pointRadius: 0
            },
            {
                label: "string",
                data: [],
                fill: false,
                borderColor: "rgb(0, 255, 0)",
                borderWidth: 1,
                tension: 0,
                pointRadius: 0
            },
            {
                label: "entity",
                data: [],
                fill: false,
                borderColor: "rgb(0, 0, 255)",
                borderWidth: 1,
                tension: 0,
                pointRadius: 0
            },
            {
                label: "handle",
                data: [],
                fill: false,
                borderColor: "rgb(0, 255, 255)",
                borderWidth: 1,
                tension: 0,
                pointRadius: 0
            },
            {
                label: "heap",
                data: [],
                fill: false,
                borderColor: "rgb(255, 255, 0)",
                borderWidth: 1,
                tension: 0,
                pointRadius: 0
            }
        ]
    },
    options: {
        animation: false
    }
});

window.addEventListener('message', event => {
    const msg = event.data
    switch (msg.type) {
        case 'hide': {
            stateChart.update('hide')
            break
        }
        case 'show': {
            stateChart.data.labels = []
            for (let index = 0; index < stateChart.data.datasets.length; index++) {
                const element = stateChart.data.datasets[index];
                element.data = []
            }
            stateChart.update('show')
            break
        }
        case 'data': {
            const datasets = stateChart.data.datasets
            if (stateChart.data.labels.length == 30) {
                for (let index = 0; index < datasets.length; index++) {
                    const element = datasets[index];
                    element.data.shift()
                }
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