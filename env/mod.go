package env

import "fmt"

var (
	git_version string
	build_time  string
)

func Version() string {
	return fmt.Sprintf("%s(%s)", git_version, build_time)
}
