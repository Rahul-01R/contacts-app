// Sends requests to the C++ backend

async function request(method, url, body) {

    try {

        const options = {
            method: method,
            headers: body
                ? { "Content-Type": "application/json" }
                : {}
        };

        if (body) {
            options.body = JSON.stringify(body);
        }

        const response = await fetch(url, options);

        const data = await response.json().catch(() => null);

        return {
            ok: response.ok,
            status: response.status,
            data: data
        };

    } catch (error) {

        console.error("Network Error:", error);

        return {
            ok: false,
            status: 0,
            data: {
                error: "network",
                message: "Cannot reach the server"
            }
        };

    }

}


// API functions

const api = {

    // Get all contacts
    listContacts: () => {
        return request("GET", "/api/contacts");
    },

    // Add a new contact
    createContact: (contact) => {
        return request("POST", "/api/contacts", contact);
    },

    // Update an existing contact
    updateContact: (id, contact) => {
        return request(
            "PUT",
            "/api/contacts/" + encodeURIComponent(id),
            contact
        );
    },

    // Delete a contact
    deleteContact: (id) => {
        return request(
            "DELETE",
            "/api/contacts/" + encodeURIComponent(id)
        );
    },

    // Search contacts
    searchExact: (query) => {
        return request(
            "GET",
            "/api/search?q=" + encodeURIComponent(query)
        );
    }

};