$baseUrl = "http://localhost:18080"

Write-Host "GET /health"
try {
    $health = Invoke-WebRequest -Uri "$baseUrl/health" -Method Get
    Write-Host $health.StatusCode $health.Content
} catch {
    Write-Host "Health failed:" $_.Exception.Message
}

Write-Host "POST /ingest"
try {
    $payload = '{"event":"hello"}'
    $ingest = Invoke-WebRequest -Uri "$baseUrl/ingest" -Method Post -Body $payload -ContentType "application/json"
    Write-Host $ingest.StatusCode $ingest.Content
} catch {
    Write-Host "Ingest failed:" $_.Exception.Message
}

Write-Host "GET /stats"
try {
    $stats = Invoke-WebRequest -Uri "$baseUrl/stats" -Method Get
    Write-Host $stats.StatusCode $stats.Content
} catch {
    Write-Host "Stats failed:" $_.Exception.Message
}
