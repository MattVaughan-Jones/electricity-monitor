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
        label: 'Power (W)',
        data: powerData.data.map(row => row.power),
      },
    ],
  }

  const options = {
    responsive: true,
    maintainAspectRatio: false,
    scales: {
      x: {
        grid: {
          color: 'white',
        },
        ticks: {
          color: 'white',
        },
        title: {
          color: 'white',
        },
      },
      y: {
        grid: {
          color: 'white',
        },
        ticks: {
          color: 'white',
        },
        title: {
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
    <div>
      <Line data={data} options={options} />
    </div>
  )
}
