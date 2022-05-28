# Data visualization

The visualization happens client side in the browser and is written in Svelte, a lightweight JavaScript framework.
This system is designed to run serverless, meaning that the cloud provider is responsible for executing the code by dynamically allocating the resources. Because of this architecture, this code was compiled and deployed on Netlify and available online.
To make the data visualization live, we made use of WebSockets. When data is added to the database, it will be directly delivered to the browser. When the node moves, the location can be tracked live without reloading the page.

## Get started

Install the dependencies...

```bash
cd sw_frontend_client
npm install
```

...then start [Rollup](https://rollupjs.org):

```bash
npm run dev
```

Navigate to [localhost:8080](http://localhost:8080). You should see the app running.

## Building and running in production mode

To create an optimised version of the app:

```bash
npm run build
```

You can run the newly built app with `npm run start`. This uses [sirv](https://github.com/lukeed/sirv), which is included in your package.json's `dependencies` so that the app will work when you deploy to platforms like [Heroku](https://heroku.com).

## Deploying to the web

### With [Netlify](https://netlify.com)
Install `netlify` if you haven't already:
```bash
npm install netlify-cli -g
```

Then, from within your project folder:

```bash
cd public
netlify deploy -p
```

### With [Vercel](https://vercel.com)

Install `vercel` if you haven't already:

```bash
npm install -g vercel
```

Then, from within your project folder:

```bash
cd public
vercel deploy --name my-project
```

### With [surge](https://surge.sh/)

Install `surge` if you haven't already:

```bash
npm install -g surge
```

Then, from within your project folder:

```bash
npm run build
surge public my-project.surge.sh
```

May 2022
– Louis de Looze