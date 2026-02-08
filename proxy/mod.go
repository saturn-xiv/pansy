package proxy

import (
	"net/http"
	"strconv"
	"time"
)

func copy_header(wrt http.ResponseWriter, res *http.Response) {
	clear(wrt.Header())
	for k, v := range res.Header {
		for _, w := range v {
			wrt.Header().Add(k, w)
		}
	}
}

func beautify_duration(d time.Duration) string {
	u, ms, s := uint64(d), uint64(time.Millisecond), uint64(time.Second)
	if d < 0 {
		u = -u
	}
	switch {
	case u < ms:
		return "0"
	case u < s:
		return strconv.FormatUint(u/ms, 10) + "ms"
	default:
		return strconv.FormatUint(u/s, 10) + "s"
	}
}

func beautify_size(s int64) string {
	switch {
	case s < 1024:
		return strconv.FormatInt(s, 10) + "B"
	case s < 1024*1024:
		return strconv.FormatInt(s/1024, 10) + "KB"
	default:
		return strconv.FormatInt(s/1024/1024, 10) + "MB"
	}
}
