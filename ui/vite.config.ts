import { defineConfig } from 'vite'
import react from '@vitejs/plugin-react'

// https://vite.dev/config/
export default defineConfig({
  plugins: [react()],
  // #region this code gets this project to work with Mui v7+
  resolve: {
    alias: {
      '@mui/styled-engine': '@mui/styled-engine-sc',
    },
  },
  // #endregion
})
