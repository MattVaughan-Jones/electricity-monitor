import { Line } from 'react-chartjs-2'

import {
  Chart as ChartJS,
  CategoryScale,
  LinearScale,
  PointElement,
  LineElement,
  Title,
  Tooltip,
  Legend,
  Colors,
} from 'chart.js'

ChartJS.register(
  CategoryScale,
  LinearScale,
  PointElement,
  LineElement,
  Title,
  Tooltip,
  Legend,
  Colors
)

export type RecordingData = {
  fileName: string
  recordingName: string
  data: {
    type: string
    timestamp: number
    voltage: number
    current: number
    power: number
    frequency: number
  }[]
}

export const Graph = ({
  inputData,
}: {
  inputData: RecordingData | undefined
}) => {
  if (!inputData) {
    return <></>
  }
  const data = {
    labels: inputData.data.map(row => row.timestamp),
    datasets: [
      {
        label: inputData.recordingName,
        data: inputData.data.map(row => row.power),
      },
    ],
  }

  const options = {
    responsive: true,
    maintainAspectRatio: false,
    scales: {
      x: {
        beginAtZero: true,
        grid: {
          color: 'white',
        },
        ticks: {
          color: 'white',
        },
        title: {
          display: true,
          text: 'Time',
          color: 'white',
        },
      },
      y: {
        beginAtZero: true,
        grid: {
          color: 'white',
        },
        ticks: {
          color: 'white',
        },
        title: {
          display: true,
          text: 'Power (W)',
          color: 'white',
        },
      },
    },
    plugins: {
      legend: {
        labels: {
          color: 'white',
        },
      },
    },
  }

  return (
    <div style={{ height: '50vh' }}>
      <Line data={data} options={options} />
    </div>
  )
}
