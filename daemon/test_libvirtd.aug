module Test_libvirtd =
  let conf = "listen_tls = 0
listen_tcp = 1
tls_port = \"16514\"
tcp_port = \"16509\"
listen_addr = \"192.168.0.1\"
mdns_adv = 1
mdns_name = \"Virtualization Host Joe Demo\"
unix_sock_group = \"libvirt\"
unix_sock_ro_perms = \"0777\"
unix_sock_rw_perms = \"0770\"
unix_sock_dir = \"/var/run/libvirt\"
auth_unix_ro = \"none\"
auth_unix_rw = \"none\"
auth_tcp = \"sasl\"
auth_tls = \"none\"
access_drivers = [ \"polkit\" ]
key_file = \"/etc/pki/libvirt/private/serverkey.pem\"
cert_file = \"/etc/pki/libvirt/servercert.pem\"
ca_file = \"/etc/pki/CA/cacert.pem\"
crl_file = \"/etc/pki/CA/crl.pem\"
tls_no_sanity_certificate = 1
tls_no_verify_certificate = 1
tls_allowed_dn_list = [\"DN1\", \"DN2\"]
sasl_allowed_username_list = [\"joe@EXAMPLE.COM\", \"fred@EXAMPLE.COM\" ]
max_clients = 5000
max_queued_clients = 1000
max_anonymous_clients = 20
min_workers = 5
max_workers = 20
prio_workers = 5
max_requests = 20
max_client_requests = 5
log_level = 3
log_filters=\"3:remote 4:event\"
log_outputs=\"3:syslog:libvirtd\"
log_buffer_size = 64
audit_level = 2
audit_logging = 1
host_uuid = \"00000000-0000-0000-0000-000000000000\"
keepalive_interval = 5
keepalive_count = 5
keepalive_required = 1
"

   test Libvirtd.lns get conf =
        { "listen_tls" = "0" }
        { "listen_tcp" = "1" }
        { "tls_port" = "16514" }
        { "tcp_port" = "16509" }
        { "listen_addr" = "192.168.0.1" }
        { "mdns_adv" = "1" }
        { "mdns_name" = "Virtualization Host Joe Demo" }
        { "unix_sock_group" = "libvirt" }
        { "unix_sock_ro_perms" = "0777" }
        { "unix_sock_rw_perms" = "0770" }
        { "unix_sock_dir" = "/var/run/libvirt" }
        { "auth_unix_ro" = "none" }
        { "auth_unix_rw" = "none" }
        { "auth_tcp" = "sasl" }
        { "auth_tls" = "none" }
        { "access_drivers"
             { "1" = "polkit" }
        }
        { "key_file" = "/etc/pki/libvirt/private/serverkey.pem" }
        { "cert_file" = "/etc/pki/libvirt/servercert.pem" }
        { "ca_file" = "/etc/pki/CA/cacert.pem" }
        { "crl_file" = "/etc/pki/CA/crl.pem" }
        { "tls_no_sanity_certificate" = "1" }
        { "tls_no_verify_certificate" = "1" }
        { "tls_allowed_dn_list"
             { "1" = "DN1"}
             { "2" = "DN2"}
        }
        { "sasl_allowed_username_list"
             { "1" = "joe@EXAMPLE.COM" }
             { "2" = "fred@EXAMPLE.COM" }
        }
        { "max_clients" = "5000" }
        { "max_queued_clients" = "1000" }
        { "max_anonymous_clients" = "20" }
        { "min_workers" = "5" }
        { "max_workers" = "20" }
        { "prio_workers" = "5" }
        { "max_requests" = "20" }
        { "max_client_requests" = "5" }
        { "log_level" = "3" }
        { "log_filters" = "3:remote 4:event" }
        { "log_outputs" = "3:syslog:libvirtd" }
        { "log_buffer_size" = "64" }
        { "audit_level" = "2" }
        { "audit_logging" = "1" }
        { "host_uuid" = "00000000-0000-0000-0000-000000000000" }
        { "keepalive_interval" = "5" }
        { "keepalive_count" = "5" }
        { "keepalive_required" = "1" }
