// const baseUri = 'http://localhost:4200';
const baseUri = '';

let editId = undefined;
let auth = {
    authenticated: false,
    token: '',
    username: '',
    message: '',
};

function refreshAuthStatus() {
    $('#auth-authenticated').text( auth.authenticated ? 'YES' : 'NO');
    $('#auth-token').text(auth.token);
    $('#auth-username').text(auth.username);
    $('#auth-message').text(auth.message);
}

function login() {
    let username = $('#username').val();
    let password = $('#password').val();

    if (username && password) {
        $.ajax(`${baseUri}/auth/login`, {
            method: 'POST',
            contentType: 'application/json',
            data: JSON.stringify({
                username: username, password: password
            }),
            success: (data) => {
                console.info('data', data);
                auth.authenticated = true;
                auth.token = data.token;
                auth.username = data.username;
                auth.message = '';
                refreshAuthStatus();
                $('#username').val('');
                $('#password').val('');
            },
            error: (xhr, msg) => {
                auth.message = msg;
                refreshAuthStatus();
            }
        });
    } else {
        auth.message = 'missing username/password';
        refreshAuthStatus();
    }
}

function logout() {
    if (auth.authenticated === false) return;

    $.ajax(`${baseUri}/auth/logout`, {
        method: 'POST',
        headers: {
            'X-Auth-Token': auth.token
        },
        contentType: 'application/json',
        data: JSON.stringify({
            token: auth.token
        }),
        success: (data) => {
            console.info('data', data);
            auth.authenticated = false;
            auth.token = '';
            auth.username = '';
            auth.message = '';
            refreshAuthStatus();
        },
        error: (xhr, msg) => {
            auth.message = msg;
            refreshAuthStatus();
        }
    });
}

function removeBook(bookId) {
    if (auth.authenticated === false){
        auth.message = 'must login';
        refreshAuthStatus();
        return;
    }
    auth.message = '';
    refreshAuthStatus();

    $.ajax(`${baseUri}/book/${bookId}`, {
        method: 'DELETE',
        headers: {
            'X-Auth-Token': auth.token
        },
        success: (data) => {
            $(`#book-${bookId}`).empty();
        },
        error: (xhr, msg) => {
            auth.message = msg;
        }
    });
}

function editBook(bookId) {
    auth.message = '';
    refreshAuthStatus();

    $.ajax(`${baseUri}/book/${bookId}`, {
        method: 'GET',
        success: (book) => {
            editId = book.id;
            $('#title').val(book.title);
            $('#author').val(book.author);
            $('#price').val(book.price);
            $('#pages').val(book.pages);
        }
    });
}

function clear() {
    editId = undefined;
    $('#title').val('');
    $('#author').val('');
    $('#price').val('');
    $('#pages').val('');
}

function saveBook() {
    if (auth.authenticated === false){
        auth.message = 'must login';
        refreshAuthStatus();
        return;
    }
    auth.message = '';
    refreshAuthStatus();

    let book = {
        title: $('#title').val(),
        author: $('#author').val(),
        price: +$('#price').val(),
        pages: +$('#pages').val(),
    };
    let uri, method;
    if (editId) {
        uri = `${baseUri}/book/${editId}`;
        method = 'PUT';
    } else {
        uri = `${baseUri}/books`;
        method = 'POST';
    }
    $.ajax(uri, {
        method: method,
        headers: {
            'X-Auth-Token': auth.token
        },
        contentType: 'application/json',
        data: JSON.stringify(book),
        success: (book) => {
            editId = undefined;
            clear();
            loadAll();
        },
        error: (xhr, msg) => {
            auth.message = msg;
        }
    });
}

function loadAll() {
    auth.message = '';
    refreshAuthStatus();

    $.getJSON(baseUri + '/books', (payload) => {
        $('#books-list').empty();

        $.each(payload, (index, book) => {
            console.info(index, 'book', book);
            let html = $(`<tr id="book-${book.id}">
                            <td>${book.id}</td>
                            <td>${book.title}</td>
                            <td>${book.author}</td>
                            <td>${book.price}</td>
                            <td>${book.pages}</td>
                            <td>
                                <button class="edit-book btn btn-info">Edit</button>
                                <button class="remove-book btn btn-danger">Remove</button>
                            </td>
                        </tr>`);

            $('#books-list').append(html);
            $(`#books-list #book-${book.id} .edit-book`).click(() => {
                editBook(book.id)
            });
            $(`#books-list #book-${book.id} .remove-book`).click(() => {
                removeBook(book.id)
            });
        });
    });
}
