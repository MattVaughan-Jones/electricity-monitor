import type { PowerData } from '../../fixtures/power-data'
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

export const Graph = ({ powerData }: { powerData: PowerData }) => {
  const data = {
    labels: powerData.data.map(row => row.timestamp),
    datasets: [
      {
        label: powerData.device,
        data: powerData.data.map(row => row.power),
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
