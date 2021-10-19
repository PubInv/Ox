package main

// Import OS and fmt packages
import (
	"bytes"
	"encoding/json"
	"fmt"
	"io/ioutil"
	"log"
	"math"
	"net/http"
	"strconv"
	"time"
)

// Square wave fourier transform
func fourier(amp float64, t float64, n int) float64 {
	a := 0.0
	for i := 1; i < n; i += 2 {
		fi := float64(i)
		a += amp*math.Sin(fi*t)/fi
	}
	return a
}

// signal.py was dropping data points so the
// signal generator has been ported to Go
func main() {
	i := 0.0
	for i < 1000 {
		fi := float64(i)
		fv := fourier(2.0, fi, 10)
		v := strconv.FormatFloat(fv, 'f', 2, 32)
		t := time.Now().UTC().Format("2006-01-02T15:04:05.000Z") // Same format as JavaScript
		fmt.Println(v)
		fmt.Println(t)

		// Create JSON payload
		payload, _ := json.Marshal(map[string]string{
			"location": "E",
			"value":    v,
			"time":     t,
		})

		// Post request
		responseBody := bytes.NewBuffer(payload)
		resp, err := http.Post("http://localhost:3020/api/pimd", "application/json", responseBody)
		if err != nil {
			log.Fatalf("An error occured %v", err)
		}
		defer resp.Body.Close()

		body, err := ioutil.ReadAll(resp.Body)
		if err != nil {
			log.Fatalln(err)
		}
		sb := string(body)
		log.Printf(sb)
		time.Sleep(100 * time.Millisecond)
		i += 0.1
	}
}
