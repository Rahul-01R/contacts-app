const API_BASE_URL = "http://127.0.0.1:8080/api";

// Common function for sending HTTP requests
async function apiRequest(endpoint, options = {}) {
    const response = await fetch(`${API_BASE_URL}${endpoint}`, {
        ...options,
        headers: {
            "Content-Type": "application/json",
            ...options.headers
        }
    });

    let result = null;

    const responseText = await response.text();

    if (responseText) {
        try {
            result = JSON.parse(responseText);
        } catch {
            result = responseText;
        }
    }

    if (!response.ok) {
        const errorMessage =
            typeof result === "object" && result !== null
                ? result.message || result.error
                : result;

        throw new Error(
            errorMessage || `Request failed with status ${response.status}`
        );
    }

    return result;
}

// All API functions are inside the api object
const api = {

    // Get all contacts
    async listContacts() {
        return await apiRequest("/contacts");
    },

    // Add a new contact
    async createContact(contactData) {
        return await apiRequest("/contacts", {
            method: "POST",
            body: JSON.stringify(contactData)
        });
    },

    // Update an existing contact
    async updateContact(id, contactData) {
        return await apiRequest(`/contacts/${encodeURIComponent(id)}`, {
            method: "PUT",
            body: JSON.stringify(contactData)
        });
    },

    // Delete a contact
    async deleteContact(id) {
        return await apiRequest(`/contacts/${encodeURIComponent(id)}`, {
            method: "DELETE"
        });
    },

    // Search contacts
    async searchExact(query) {
        return await apiRequest(
            `/search?q=${encodeURIComponent(query)}`
        );
    },

    // Check whether the backend is running
    async checkServer() {
        return await apiRequest("/contacts");
    }
};