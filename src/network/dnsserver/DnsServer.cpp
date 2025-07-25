#include <cstdint>
#include "DnsServer.h"

#include <cstring>
#include <string>
#include <src/config.h>

#include "DnsTable.h"

// Real flags implementation, but I don't care about them since I want to send everything on single IP
typedef struct
{
#if BYTE_ORDER == LITTLE_ENDIAN
    std::uint8_t rd : 1, /* Recursion Desired */
        tc : 1, /* Truncation Flag */
        aa : 1, /* Authoritative Answer Flag */
        opcode : 4, /* Operation code */
        qr : 1; /* Query/Response Flag */
    uint8_t rcode : 4, /* Response Code */
        z : 3, /* Zero */
        ra : 1; /* Recursion Available */
#else
    uint8_t qr: 1,     /* Query/Response Flag */
            opcode: 4, /* Operation code */
            aa: 1,     /* Authoritative Answer Flag */
            tc: 1,     /* Truncation Flag */
            rd: 1;     /* Recursion Desired */
    uint8_t ra: 1,     /* Recursion Available */
            z: 3,      /* Zero */
            rcode: 4;  /* Response Code */
#endif
} dns_header_flags_t;

DnsServer::DnsServer()
{
    control_block = udp_new();
}


struct dns_header_t
{
    uint16_t id;
    uint16_t flags;
    uint16_t questions;
    uint16_t answers;
    uint16_t authority;
    uint16_t additional;

    explicit dns_header_t(const uint8_t* dns_data)
    {
        id = dns_data[0] << 8 | dns_data[1];
        flags = dns_data[2] << 8 | dns_data[3];
        questions = dns_data[4] << 8 | dns_data[5];
        answers = dns_data[6] << 8 | dns_data[7];
        authority = dns_data[8] << 8 | dns_data[9];
        additional = dns_data[10] << 8 | dns_data[11];
    }
};

typedef struct __attribute__((packed)) dns_answer
{
    uint16_t name;
    uint16_t type;
    uint16_t Class;
    uint32_t ttl;
    uint16_t len;
    uint32_t addr;
} dns_answer_t;

typedef struct dns_query
{
    char name[DNS_MAX_HOST_NAME_LEN];
    uint16_t type;
    uint16_t Class;
} dns_query_t;

int parse_domain_name(const uint8_t* data, const int max_len, uint16_t* type, uint16_t* Class, char* domain_name)
{
    int pos = sizeof(dns_header_t);
    int domain_name_pos = 0;

    while (pos < max_len)
    {
        const uint8_t len = data[pos++];

        if (len == 0)
        {
            *type = data[pos] << 8 | data[pos + 1];
            *Class = data[pos + 2] << 8 | data[pos + 3];
            return pos + 4;
        }

        if (len >= 0xC0)
        {
#if DNS_DEBUG
            printf("DNS compression not supported\n");
#endif
            return -1;
        }

        if (pos + len >= max_len) return -1;

        for (int i = 0; i < len; i++)
        {
            domain_name[domain_name_pos++] = data[pos++];
        }
        domain_name[domain_name_pos++] = '.';
    }

    if (domain_name_pos > 0) {
        domain_name[domain_name_pos - 1] = '\0';
    }
    return -1;
}

// Function to create IPv4 (A record) response
pbuf* create_dns_response_ipv4(const uint8_t* original_query, const int query_len, const ip4_addr_t* ip_address)
{
    const uint response_size = query_len + sizeof(dns_answer_t);
    pbuf* response = pbuf_alloc(PBUF_TRANSPORT, response_size, PBUF_RAM);
    auto* resp_data = static_cast<uint8_t*>(response->payload);

    // 1. Copy original query
    memcpy(resp_data, original_query, query_len);

    // 2. Modify header flags
    resp_data[2] = 0x84; // Response + Authoritative
    resp_data[3] = 0x00; // No error

    resp_data[6] = 0x00;
    resp_data[7] = 0x01; // 1 answer

    // 3. Fill answer struct for A record
    dns_answer_t answer;
    answer.name = htons(0xC00C);  // Pointer to question name
    answer.type = htons(1);       // A record (IPv4)
    answer.Class = htons(1);      // IN class
    answer.ttl = htonl(300);      // 5 minutes TTL
    answer.len = htons(4);        // 4 bytes for IPv4
    answer.addr = htonl(ip4_addr_to_uint32(ip_address));

    // 4. Append answer (exactly at query_len, no extra bytes!)
    memcpy(resp_data + query_len, &answer, sizeof(answer));

    return response;
}

// Function to create NXDOMAIN (rejection) response
pbuf* create_dns_response_nxdomain(const uint8_t* original_query, const int query_len)
{
    // Response is same size as query (no answer section)
    pbuf* response = pbuf_alloc(PBUF_TRANSPORT, query_len, PBUF_RAM);
    auto* resp_data = static_cast<uint8_t*>(response->payload);

    // Copy original query
    memcpy(resp_data, original_query, query_len);

    // Modify header flags for NXDOMAIN
    resp_data[2] = 0x84; // Response + Authoritative
    resp_data[3] = 0x03; // NXDOMAIN error code
    resp_data[7] = 0x00; // 0 answers

    return response;
}

void print_pbuf_bytes(struct pbuf* p) {
    struct pbuf* current = p;
    
    while (current != NULL) {
#if DNS_DEBUG            
        uint8_t* payload = (uint8_t*)current->payload;
        
        for (int i = 0; i < current->len; i++) {
            printf("%02X ", payload[i]);
        }
#endif
        
        current = current->next;
    }
#if DNS_DEBUG    
    printf("\n");
#endif
}

// Updated main UDP callback with proper type handling
udp_recv_fn DnsServer::udp_process_request_function = [](void* arg, struct udp_pcb* control_block, struct pbuf* package,
                                                         const ip_addr_t* sender_ip, const u16_t client_port) -> void
{
    [[maybe_unused]] auto context = static_cast<DnsServer*>(arg);

#if DNS_DEBUG
    printf("🔍 DNS: Request from %s:%d\n", ip4addr_ntoa(ip_2_ip4(sender_ip)), client_port);
#endif

    const auto package_data = static_cast<uint8_t*>(package->payload);
    const auto package_len = package->len;
    const auto package_header = dns_header_t(package_data);

    if (package_header.flags & 0x8000) {
#if DNS_DEBUG
        printf("Ignoring response packet\n");
#endif
        pbuf_free(package);
        return;
    }

    dns_query query{};
    parse_domain_name(package_data, package_len, &query.type, &query.Class, query.name);

    pbuf* response;

    if (query.type == 1) {  // A record (IPv4) - WE CAN HANDLE THIS
        ip4_addr_t result_address = CONFIG::AP_IP;
        
        // Check if we have specific mapping
        const auto pair = ip4_addresses.find(query.name);
        if (pair != ip4_addresses.end()) {
            result_address = pair->second;
        }
        
        response = create_dns_response_ipv4(package_data, package_len, &result_address);
#if DNS_DEBUG        
        printf("DNS Response: %s -> %s\n", query.name, ip4addr_ntoa(&result_address));
#endif
        
    } else if (query.type == 28) {  // AAAA record (IPv6) - REJECT
#if DNS_DEBUG
        printf("❌ Rejecting IPv6 query (we don't support IPv6)\n");
#endif
        response = create_dns_response_nxdomain(package_data, package_len);
        
    } else {  // Other types - REJECT
#if DNS_DEBUG
        printf("❌ Rejecting unsupported query type %d\n", query.type);
#endif
        response = create_dns_response_nxdomain(package_data, package_len);
    }

    udp_sendto(control_block, response, sender_ip, client_port);
    pbuf_free(response);
    pbuf_free(package);
};

pbuf* create_dns_response(const uint8_t* original_query, const int query_len, const ip4_addr_t* ip_address)
{
    const uint response_size = query_len + sizeof(dns_answer_t);

    pbuf* response = pbuf_alloc(PBUF_TRANSPORT, response_size, PBUF_RAM);
    auto* resp_data = static_cast<uint8_t*>(response->payload);

    // 1. Copy original query
    memcpy(resp_data, original_query, query_len);

    // 2. Modify header flags
    resp_data[2] = 0x84; // Response + Authoritative
    resp_data[3] = 0x00; // No error
    resp_data[7] = 0x01; // 1 answer

    // 3. Fill answer struct
    dns_answer_t answer;
    answer.name = htons(0xC00C);
    answer.type = htons(1);
    answer.Class = htons(1);
    answer.ttl = htonl(300);
    answer.len = htons(4);

    answer.addr = htonl(ip4_addr_to_uint32(ip_address));

    // 4. Append answer
    memcpy(resp_data + query_len, &answer, sizeof(answer));

    return response;
}

DnsServer::~DnsServer()
{
    deinit();
}

void DnsServer::deinit()
{
    if (this->control_block != nullptr)
    {
        udp_disconnect(this->control_block);
        this->control_block = nullptr;
#if DNS_DEBUG        
        printf("DNS: Server deinitialized\n");
#endif
    }
}

static int bind_callback(udp_pcb** udp, void* cb_data, const udp_recv_fn cb_udp_recv)
{
    *udp = udp_new();
    if (*udp == nullptr)
    {
        return -ENOMEM;
    }
    udp_recv(*udp, cb_udp_recv, cb_data);
    return ERR_OK;
}

static int ip_port_dns_bind(udp_pcb** udp)
{
    const err_t err = udp_bind(*udp, &CONFIG::DNS_ADDRESS, CONFIG::DNS_PORT);
    if (err != ERR_OK)
    {
#if DNS_DEBUG
        printf("dns failed to bind to port %u: %d", CONFIG::DNS_PORT, err);
#endif
        return 1;
    }
    return err;
}

int DnsServer::init()
{
    if (bind_callback(&this->control_block, this, *udp_process_request_function) != ERR_OK)
    {
#if DNS_DEBUG
        printf("dns server failed to start\n");
#endif
        return 1;
    }
    if (ip_port_dns_bind(&this->control_block) != ERR_OK)
    {
#if DNS_DEBUG
        printf("dns server failed to bind\n");
#endif
        return 1;
    }

#if DNS_DEBUG
    printf("DNS server listening on port %d\n", CONFIG::DNS_PORT);
#endif
    return 0;
}
