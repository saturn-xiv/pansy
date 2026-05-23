package app

import (
	"log"
	"log/slog"
	"os"
	"os/user"
	"path/filepath"

	"github.com/spf13/cobra"
	"github.com/spf13/viper"

	"github.com/saturn-xiv/pansy/env"
)

var (
	gl_config_file  string
	gl_debug        bool
	gl_http_port    uint16
	gl_http_host    string
	gl_ssh_host     string
	gl_ssh_port     uint16
	gl_ssh_user     string
	gl_ssh_key_file string

	gl_root_cmd = &cobra.Command{
		Use:   "pansy",
		Short: "launch a HTTP proxy server over SSH.",
		Example: `  1: Generating a new key:
  ssh-keygen -t ed25519 -C 'your_email@example.com' -f .ssh
  2: Start the proxy server:
  pansy -d -H <remote-host> -P <remote-port> -U <remote-user> -K $PWD/.ssh -p <local-port>
`,
		Version: env.Version(),
		Run: func(cmd *cobra.Command, args []string) {
			ssh := Ssh{
				Host:    gl_ssh_host,
				Port:    gl_ssh_port,
				User:    gl_ssh_user,
				KeyFile: gl_ssh_key_file,
			}
			if err := ssh.StartHttpProxyServer(gl_http_host, gl_http_port); err != nil {
				log.Fatal(err)
			}
		},
	}
)

func Execute() error {
	return gl_root_cmd.Execute()
}

func init() {
	cobra.OnInitialize(init_logger, init_config)

	user, err := user.Current()
	if err != nil {
		log.Fatalln(err)
	}
	home, err := os.UserHomeDir()
	if err != nil {
		log.Fatalln(err)
	}
	gl_root_cmd.PersistentFlags().StringVarP(&gl_config_file, "config", "c", "config.toml", "configuration file")
	gl_root_cmd.PersistentFlags().BoolVarP(&gl_debug, "debug", "d", false, "run on debug mode")
	gl_root_cmd.PersistentFlags().StringVar(&gl_http_host, "host", "0.0.0.0", "ip address for local http proxy server listen to")
	gl_root_cmd.PersistentFlags().Uint16VarP(&gl_http_port, "port", "p", 8080, "port for local http proxy server listen to")
	gl_root_cmd.PersistentFlags().StringVarP(&gl_ssh_host, "ssh-host", "H", "127.0.0.1", "ssh host")
	gl_root_cmd.PersistentFlags().Uint16VarP(&gl_ssh_port, "ssh-port", "P", 22, "ssh port")
	gl_root_cmd.PersistentFlags().StringVarP(&gl_ssh_user, "ssh-user", "U", user.Username, "ssh user")
	gl_root_cmd.PersistentFlags().StringVarP(&gl_ssh_key_file, "ssh-key-file", "K", filepath.Join(home, ".ssh", "id_ed25519"), "ssh private key file")
}

func init_logger() {
	if gl_debug {
		slog.SetLogLoggerLevel(slog.LevelDebug)
	} else {
		slog.SetLogLoggerLevel(slog.LevelInfo)
	}
	slog.Debug("run on debug mode")
}

func init_config() {
	if gl_config_file != "" {
		viper.SetConfigFile(gl_config_file)
	} else {
		home, err := os.UserHomeDir()
		cobra.CheckErr(err)
		viper.AddConfigPath(home)
		viper.SetConfigType("toml")
		viper.SetConfigName(".pansy")
	}

	viper.AutomaticEnv()
	if err := viper.ReadInConfig(); err == nil {
		slog.Debug("using", "file", viper.ConfigFileUsed())
	}
}
